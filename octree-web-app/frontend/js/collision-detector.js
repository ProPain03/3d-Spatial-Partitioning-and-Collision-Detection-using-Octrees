// filepath: /octree-web-app/octree-web-app/frontend/js/collision-detector.js
const COLLISION_SIZE = 30;

function detectCollision(point, points) {
    for (let i = 0; i < points.length; i++) {
        const otherPoint = points[i];
        const distance = Math.sqrt(
            Math.pow(point.x - otherPoint.x, 2) +
            Math.pow(point.y - otherPoint.y, 2) +
            Math.pow(point.z - otherPoint.z, 2)
        );

        if (distance < COLLISION_SIZE) {
            return otherPoint; // Return the colliding point
        }
    }
    return null; // No collision detected
}

function visualizeCollision(point, collidingPoint) {
    // Highlight the colliding points in the 3D scene
    if (collidingPoint) {
        // Assuming you have a method to highlight points
        highlightPoint(point, 'red');
        highlightPoint(collidingPoint, 'red');
    }
}

function highlightPoint(point, color) {
    // Implement the logic to change the color of the point in the Three.js scene
    // This could involve changing the material of the mesh representing the point
}