#include "ships.h"

// Ships are shaped like elongated diamonds
b2Vec2 shipPoints[] = {
    {0.0f, 1.0f},   // Top point (elongated)
    {0.5f, 0.3f},   // Top-right corner
    {0.5f, -0.3f},  // Bottom-right corner
    {0.0f, -1.0f},  // Bottom point (elongated)
    {-0.5f, -0.3f}, // Bottom-left corner
    {-0.5f, 0.3f},  // Top-left corner
    {0.0f, 1.0f}    // Close the loop
};

Ship createShip(b2WorldId worldId, b2Vec2 position, b2Vec2 target) {
    Ship ship;

    // Copy the ship points to the ship struct
    memcpy(ship.points, shipPoints, sizeof(shipPoints));

    // Define the dynamic body for the ship
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    ship.bodyId = b2CreateBody(worldId, &bodyDef);

    // define the hull of the ship
    b2Hull hull = b2ComputeHull(shipPoints, NUMBER_OF_POINTS);
    float radius = 0.5f; // dont know what this is

    // Define box shape
    ship.width = .5f;
    ship.height = .5f;
    b2Polygon dynamicBox = b2MakePolygon(&hull, radius);

    // Define shape properties
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    shapeDef.restitution = .20f;

    // Attach shape to the body
    b2CreatePolygonShape(ship.bodyId, &shapeDef, &dynamicBox);

    // Initialize motor forces
    ship.leftMotorForce = 1.0f;
    ship.rightMotorForce = 1.0f;


    // generate random colors for each ship
    ship.color = (Color){rand() % 255, rand() % 255, rand() % 255, 255};

    ship.target = target;
    ship.targetOrientation = (b2Rot){1.0f, 0.0f};

    return ship;
}

void createShips(b2WorldId worldId, int numberOfShips, Ship* ships) {
    int xpos = -30;
    int ypos = -25;


    for (int i = 0; i < numberOfShips; i++) {
        xpos = i % 2 == 0 ? -30 : 30;
        if(i % 2 == 0) {
            ypos += 2;
        }
        b2Vec2 position = {xpos, ypos};
        b2Vec2 target = {0.0f + i * 5, 0.0f + i * 5};
        ships[i] = createShip(worldId, position, target);  // Initialize each ship at a specified position
    }
}

b2Vec2 subtract(b2Vec2 a, b2Vec2 b) {
    b2Vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

b2Vec2 scale(b2Vec2 v, float scalar) {
    b2Vec2 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    return result;
}

b2Vec2 add(b2Vec2 a, b2Vec2 b) {
    b2Vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

// move the ship towards the target and stop when it reaches the target
void moveShipsToTarget(int numberOfShips, Ship* ships) {
    for (int i = 0; i < numberOfShips; i++) {
        Ship ship = ships[i];
        b2BodyId bodyId = ship.bodyId;
        b2Vec2 position = b2Body_GetPosition(bodyId);

        // Calculate the vector from the ship to the target
        b2Vec2 direction = subtract(ship.target, position);
        float distance = b2Length(direction);

        // Normalize the direction vector
        if (distance > 0.0f) {
            direction = scale(direction, 1.0f / distance);
        }

        // Get the current velocity of the ship
        b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
        float speed = b2Length(velocity);

        // Calculate the required stopping distance
        float brakingForce = 3.0f;  // Braking force constant
        float stoppingDistance = (speed * speed) / (2.0f * brakingForce);

        if (distance <= stoppingDistance) {
            // Braking phase: Apply a force opposite to the velocity
            b2Vec2 brakingForceVec = scale(velocity, -1.0f); // Opposite to velocity
            brakingForceVec = scale(brakingForceVec, brakingForce);
            b2Body_ApplyForceToCenter(bodyId, brakingForceVec, true);
        } else {
            // Acceleration phase: Apply a force towards the target
            float maxForce = 10.0f;
            b2Vec2 force = scale(direction, maxForce);
            b2Body_ApplyForceToCenter(bodyId, force, true);
        }

        // Small threshold to stop the ship completely
        if (distance < 0.5f && speed < 0.1f) {
            b2Body_SetLinearVelocity(bodyId, (b2Vec2){0.0f, 0.0f});
        }
    }
}

void moveShipsToTargetPID(int numberOfShips, Ship* ships, b2Vec2 target) {
    static float previousError[100] = {0.0f}; // Per-ship error tracking
    static float integral[100] = {0.0f};     // Per-ship integral tracking

    float Kp = 1.0f;  // Proportional gain
    float Ki = 0.1f;  // Integral gain
    float Kd = 1.0f;  // Derivative gain
    float brakingFactor = 1.5f; // Braking factor to counter velocity

    for (int i = 0; i < numberOfShips; i++) {
        b2BodyId bodyId = ships[i].bodyId;
        b2Vec2 position = b2Body_GetPosition(bodyId);
        b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);

        // Calculate error (distance to target)
        b2Vec2 direction = subtract(target, position);
        float error = b2Length(direction);

        // Stop if close enough to the target
        if (error < 0.5f && b2Length(velocity) < 0.1f) {
            b2Body_SetLinearVelocity(bodyId, (b2Vec2){0.0f, 0.0f});
            continue;
        }

        // Normalize direction vector
        if (error > 0.0f) {
            direction = scale(direction, 1.0f / error);
        }

        // Proportional term
        float proportional = Kp * error;

        // Integral term
        integral[i] += error;
        float integralTerm = Ki * integral[i];

        // Derivative term
        float derivative = error - previousError[i];
        float derivativeTerm = Kd * derivative;

        // Total PID force
        float forceMagnitude = proportional + integralTerm + derivativeTerm;

        // Apply braking near the target
        if (error < 5.0f) { // Braking zone
            b2Vec2 brakingForce = scale(velocity, -brakingFactor); // Opposite to velocity
            forceMagnitude = fminf(forceMagnitude, 10.0f); // Limit force magnitude for stability
            b2Vec2 force = scale(direction, forceMagnitude);
            force = add(force, brakingForce);
            b2Body_ApplyForceToCenter(bodyId, force, true);
        } else {
            // Regular movement towards the target
            b2Vec2 force = scale(direction, forceMagnitude);
            b2Body_ApplyForceToCenter(bodyId, force, true);
        }

        // Update previous error
        previousError[i] = error;
    }
}
