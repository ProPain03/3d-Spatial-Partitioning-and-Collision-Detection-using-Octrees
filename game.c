// main.c
#include "octree.h"
#include <stdio.h>
#include <stdlib.h>

// Function to handle the game loop for moving points
void gameLoop(OctreeNode *root, Point *selectedPoint) {
    char command;
    OctreeNode *selectedNode = searchPoint(root, selectedPoint);
    if (selectedNode == NULL) {
        printf("Selected point not found in the octree.\n");
        return;
    }
    int index = -1;
    for (int i = 0; i < selectedNode->ptCount; i++) {
        if (selectedNode->points[i].x == selectedPoint->x &&
            selectedNode->points[i].y == selectedPoint->y &&
            selectedNode->points[i].z == selectedPoint->z) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Point index not found in the node.\n");
        return;
    }

    while (1) {
        bool isChanged = true;
        printf("\nCurrent point: (%.2f, %.2f, %.2f)\n", selectedPoint->x, selectedPoint->y, selectedPoint->z);
        printf("Enter command (w,a,s,d,e,f for movement, n for nearest neighbor, q to quit): ");
        command = getchar();
        getchar();  // Consume newline

        Point oldPoint = *selectedPoint;

        switch (command) {
            case 'w': selectedPoint->y += STEP;
                if (selectedPoint->y > MAX_SIZE){
                    isChanged = false;
                    printf("Can't go out of the bounds\n");
                    selectedPoint->y -= STEP;}
                break;
            case 's': selectedPoint->y -= STEP;
                if (selectedPoint->y < -MAX_SIZE){
                    isChanged = false;
                    printf("Can't go out of the bounds\n");
                    selectedPoint->y += STEP;}
                break;
            case 'd': selectedPoint->x += STEP;
                if (selectedPoint->x > MAX_SIZE){
                    isChanged = false;
                    printf("Can't go out of the bounds\n");
                    selectedPoint->x -= STEP;}
                break; 
            case 'a': selectedPoint->x -= STEP;
                if (selectedPoint->x < -MAX_SIZE){
                    isChanged = false;
                    printf("Can't go out of the bounds\n");
                    selectedPoint->x += STEP;}
                break;
            case 'e': selectedPoint->z += STEP; 
                if (selectedPoint->z > MAX_SIZE){
                    isChanged = false;
                    printf("Can't go out of the bounds\n");
                    selectedPoint->z -= STEP;}
                break;
            case 'f': selectedPoint->z -= STEP; 
                if (selectedPoint->z < -MAX_SIZE){
                    isChanged = false;
                    printf("Can't go out of the bounds\n");
                    selectedPoint->z += STEP;}
                break;
            case 'n': {
                Point nearest;
                float distance;
                if (findNearestNeighbor(root, *selectedPoint, &nearest, &distance)) {
                    printf("Nearest neighbor: (%.2f, %.2f, %.2f) at distance %.2f\n",
                           nearest.x, nearest.y, nearest.z, distance);
                } else {
                    printf("No nearest neighbor found.\n");
                }
                continue;
            }
            case 'q': return;
            default: printf("Invalid command.\n"); continue;
        }
        // If new point is out of max size, then print can't go out of the max size
        if (isChanged) {
            // Check for collision
            if (detect_collision(root, *selectedPoint, COLLISION_SIZE)) {
                *selectedPoint = oldPoint;
                printf("Collision detected. Reverting to old position.\n");
            }
            else{
                updatePointInTree(root, &oldPoint, selectedPoint);
                printTree(root);
            }
        }    
    }
}

int main() {
    // Initialize the root of the octree
    Point initialcenter = {0.0f, 0.0f, 0.0f};
    float size = MAX_SIZE;  // Arbitrary root size
    OctreeNode *root = createNode(initialcenter, size, 0);

    // Read points from file
    readPoints("random1.txt", root);

    // Print the octree to Octree.txt
    printTree(root);    
    printf("Octree structure has been written to Octree.txt\n");

    // Select a point for manipulation
    Point selectedPoint;
    printf("\nEnter initial point coordinates to select (x y z): ");
    if (scanf("%f %f %f", &selectedPoint.x, &selectedPoint.y, &selectedPoint.z) != 3) {
        fprintf(stderr, "Invalid input for point coordinates.\n");
        freeTree(root);
        return EXIT_FAILURE;
    }
    getchar();  // Consume newline

    printf("Selected Point: (%.2f, %.2f, %.2f)\n", selectedPoint.x, selectedPoint.y, selectedPoint.z);
    OctreeNode *selectedNode = searchPoint(root, &selectedPoint);
    if (selectedNode == NULL){
        printf("Point not found in the octree\n");
    } else{
        gameLoop(root, &selectedPoint);
    }
    // Clear input buffer
    while (getchar() != '\n');

    // Perform range query
    Point minCube, maxCube;
    printf("\n\nGive the range of the cube for range query \n");
    printf("Enter the minimum corner of the cube (x y z): ");
    if (scanf("%f %f %f", &minCube.x, &minCube.y, &minCube.z) != 3) {
        fprintf(stderr, "Invalid input for minimum corner.\n");
        freeTree(root);
        return EXIT_FAILURE;
    }
    printf("Enter the maximum corner of the cube (x y z): ");
    if (scanf("%f %f %f", &maxCube.x, &maxCube.y, &maxCube.z) != 3) {
        fprintf(stderr, "Invalid input for maximum corner.\n");
        freeTree(root);
        return EXIT_FAILURE;
    }

    // Open RangeQuery.txt for writing range query results
    FILE *fq = fopen("RangeQuery.txt", "w");
    if (!fq) {
        perror("Failed to open RangeQuery.txt for writing");
        freeTree(root);
        return EXIT_FAILURE;
    }

    printf("Points within the specified cube have been written to RangeQuery.txt\n");
    int count = 0;
    rangeQuery(root, &minCube, &maxCube, &count, fq);
    fclose(fq);
    printf("Total points within the cube: %d\n", count);

    // Free allocated memory
    freeTree(root);

    return 0;
}
