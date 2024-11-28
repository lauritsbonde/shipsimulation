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
    float radius = 0.0f; // Small rounding radius for corners
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


    // Generate random colors for each ship
    ship.color = (Color){rand() % 255, rand() % 255, rand() % 255, 255};

    b2Rot intialOrientation = {0.0f, 1.0f}; // Set the initial orientation as needed

    b2Body_SetTransform(ship.bodyId, position, intialOrientation);

    // Set motor positions symmetrically, one on each side of the ship
    ship.leftMotorPosition = rotate((b2Vec2){ 0.50f, 0.0f}, intialOrientation);
    ship.rightMotorPosition = rotate((b2Vec2){ -0.50f, 0.0f}, intialOrientation);

    // Set target and target orientation
    ship.targetOrientation = (b2Rot){0.0f, 1.0f};

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

    // Normalize the vector to get the direction only
    float length = sqrtf(vectorToTarget.x * vectorToTarget.x + vectorToTarget.y * vectorToTarget.y);
    if (length > 0.0f) {
        vectorToTarget.x /= length;
        vectorToTarget.y /= length;
    }

    return vectorToTarget;
}

float calculateTargetAngle(b2Vec2 forwardDirection, b2Vec2 vectorToTarget) {
    float targetAngle = atan2f(forwardDirection.x * vectorToTarget.y - forwardDirection.y * vectorToTarget.x, forwardDirection.x * vectorToTarget.x + forwardDirection.y * vectorToTarget.y);

    // Normalize the angle to be between -pi and pi
    if (targetAngle > M_PI) {
        targetAngle -= 2.0f * M_PI;
    } else if (targetAngle < -M_PI) {
        targetAngle += 2.0f * M_PI;
    }

    return targetAngle;
}

void adjustMotorForces(Ship* ship, float targetAngle) {
    if (targetAngle > 0.1f) {
        ship->leftMotorForce = 0.0f;
        ship->rightMotorForce = 1.0f;
    } else if (targetAngle < -0.1f) {
        ship->leftMotorForce = 1.0f;
        ship->rightMotorForce = 0.0f;
    } else {
        ship->leftMotorForce = 1.0f;
        ship->rightMotorForce = 1.0f;
    }
}

void applyMotorForces(Ship* ship, b2BodyId bodyId, b2Vec2 position, b2Rot orientation) {
    // Get the positions of the motors
    b2Vec2 leftMotorPosition = add(position, rotate(ship->leftMotorPosition, orientation));
    b2Vec2 rightMotorPosition = add(position, rotate(ship->rightMotorPosition, orientation));

    printf("leftMotorPosition = %f, %f, rightMotorPosition = %f, %f, shipPosition = %f, %f\n", leftMotorPosition.x, leftMotorPosition.y, rightMotorPosition.x, rightMotorPosition.y, position.x, position.y);

    // Apply the forces in the direction of the forward vector
    b2Vec2 localForwardForce = {ship->leftMotorForce, 0};  // Forward force
    b2Vec2 localBackwardForce = {ship->rightMotorForce, 0}; // Backward force

    // Rotate the forces according to the ship's current rotation
    b2Vec2 leftMotorForce = rotate(localForwardForce, orientation);
    b2Vec2 rightMotorForce = rotate(localBackwardForce, orientation);

    printf("leftMotorForce = %f, %f, rightMotorForce = %f, %f\n", leftMotorForce.x, leftMotorForce.y, rightMotorForce.x, rightMotorForce.y);

    // Apply forces to the motors
    b2Body_ApplyForce(bodyId, leftMotorForce, leftMotorPosition, false);
    b2Body_ApplyForce(bodyId, rightMotorForce, rightMotorPosition, false);
}


void moveShipsToTarget(int numberOfShips, Ship* ships) {
    for (int i = 0; i < numberOfShips; i++) {
        Ship* ship = &ships[i];
        b2BodyId bodyId = ship->bodyId;
        
        // Get ship information
        b2Vec2 position = b2Body_GetPosition(bodyId);
        b2Rot orientation = b2Body_GetRotation(bodyId);
        float angle = b2Rot_GetAngle(orientation);
        b2Vec2 forwardDirection = {cosf(angle), sinf(angle)};

        // Calculate the vector to the target
        b2Vec2 vectorToTarget = calculateVectorToTarget(ship);
        
        // Calculate the target angle
        float targetAngle = calculateTargetAngle(forwardDirection, vectorToTarget);

        // Adjust motor forces based on the target angle
        adjustMotorForces(ship, targetAngle);

        // Apply forces to the motors
        applyMotorForces(ship, bodyId, position, orientation);
    }
}

