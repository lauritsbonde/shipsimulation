#include "ships.h"

Ship createShip(b2WorldId worldId, b2Vec2 position) {
    Ship ship;

    // Copy the ship points to the ship struct
    memcpy(ship.points, shipPoints, NUMBER_OF_POINTS * sizeof(b2Vec2));

    // Define the dynamic body for the ship
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    ship.bodyId = b2CreateBody(worldId, &bodyDef);

    // Define the hull of the ship
    b2Hull hull = b2ComputeHull(shipPoints, NUMBER_OF_POINTS);

    // Define the polygon shape with a small radius for smooth edges
    ship.width = 1.0f;  // Full width of the ship
    ship.height = 2.0f; // Full height of the ship
    float radius = 0.1f; // Small rounding radius for corners
    b2Polygon dynamicBox = b2MakePolygon(&hull, radius);

    // Define shape properties
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.5f; // Higher friction to reduce sliding
    shapeDef.restitution = 0.1f; // Lower restitution for stable behavior

    // Attach shape to the body
    b2CreatePolygonShape(ship.bodyId, &shapeDef, &dynamicBox);

    // Initialize motor forces
    ship.leftMotorForce = 1.0f;
    ship.rightMotorForce = 1.0f;

    // Set motor positions symmetrically
    ship.leftMotorPosition = (b2Vec2){-ship.width / 2.0f, 0.0f};  // Mid-left edge
    ship.rightMotorPosition = (b2Vec2){ship.width / 2.0f, 0.0f}; // Mid-right edge

    // Generate random colors for each ship
    ship.color = (Color){rand() % 255, rand() % 255, rand() % 255, 255};

    // Set target and target orientation
    ship.targetOrientation = (b2Rot){1.0f, 0.0f};

    return ship;
}


void createShips(b2WorldId worldId, int numberOfShips, Ship* ships) {
    int xpos = -30;
    int ypos = -25;


    for (int i = 0; i < numberOfShips; i++) {
        xpos = i % 2 == 0 ? -30 : 30;
        if(i % 2 == 0) {
            ypos += 4;
        }
        b2Vec2 position = {xpos, ypos};
        ships[i] = createShip(worldId, position);  // Initialize each ship at a specified position
    }
}

b2Vec2 calculateVectorToTarget(Ship* ship) {
    // Get the ship's body and position
    b2BodyId bodyId = ship->bodyId;
    b2Vec2 currentPosition = b2Body_GetPosition(bodyId);

    // Get the target position
    b2Vec2 targetPosition = ship->target;

    // Calculate the vector to the target
    b2Vec2 vectorToTarget = {
        .x = targetPosition.x - currentPosition.x,
        .y = targetPosition.y - currentPosition.y
    };

    // Optional: Normalize the vector if you want the direction only
    float length = sqrtf(vectorToTarget.x * vectorToTarget.x + vectorToTarget.y * vectorToTarget.y);
    if (length > 0.0f) {
        vectorToTarget.x /= length;
        vectorToTarget.y /= length;
    }

    return vectorToTarget;
}

// move the ship towards the target and stop when it reaches the target
void moveShipsToTarget(int numberOfShips, Ship* ships) {
    for (int i = 0; i < numberOfShips; i++) {
        Ship* ship = &ships[i];
        b2BodyId bodyId = ship->bodyId;
        
        // get ship information
        b2Vec2 position = b2Body_GetPosition(bodyId);
        b2Rot orientation = b2Body_GetRotation(bodyId);
        b2Vec2 forwardDirection = {orientation.c, orientation.s};

        // calculate the angle to the target
        b2Vec2 targetVector = calculateVectorToTarget(ship);
        float angleToTarget = atan2f(targetVector.y, targetVector.x) - atan2f(forwardDirection.y, forwardDirection.x);
        if (angleToTarget > M_PI) angleToTarget -= 2.0f * M_PI;
        if (angleToTarget < -M_PI) angleToTarget += 2.0f * M_PI;

        float baseForce = 10.0f; // Adjust for forward thrust
        float turnAdjustment = angleToTarget * 5.0f; // Proportional turning

        float leftForce = baseForce - turnAdjustment;
        float rightForce = baseForce + turnAdjustment;

        b2Vec2 leftForceVector = scale(targetVector, ship->leftMotorForce);
        b2Vec2 rightForceVector = scale(targetVector, ship->rightMotorForce);


        // Calculate motor positions in world space
        b2Vec2 leftMotorWorldPosition = add(position, ship->leftMotorPosition);
        b2Vec2 rightMotorWorldPosition = add(position, ship->rightMotorPosition);

        // Apply the same force to both motors
        b2Body_ApplyForce(bodyId, leftForceVector, leftMotorWorldPosition, true);
        b2Body_ApplyForce(bodyId, rightForceVector, rightMotorWorldPosition, true);
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
