#include "octree.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    Point initialcenter = {0.0f, 0.0f, 0.0f};
    float size = MAX_SIZE;  // Arbitrary root size
    OctreeNode *root = createNode(initialcenter, size, 0);

    //Take input query from user for insertion, deletion, search, range query, nearest neighbor search, print tree, free tree, collision detection
    char query;
    Point *selectedPoint = (Point *)malloc(sizeof(Point));
    Point *oldPoint = (Point *)malloc(sizeof(Point));
    if (!selectedPoint || !oldPoint) {
        perror("Failed to allocate memory for point");
        exit(EXIT_FAILURE);
    }
    *selectedPoint = initialcenter;
    *oldPoint = initialcenter;
    //open file for output of range query
    FILE *fp = fopen("RangeQuery.txt", "w");
    if (!fp) {
        perror("Failed to open RangeQuery.txt for writing");
        return 1;
    }

    while (1) {
        printf("Enter command (i: insert, d: delete, s: search, r: range query, n: nearest neighbor, q: quit): ");
        scanf(" %c", &query);
        bool isChanged = true;
        switch (query) {
            case 'i': {
                //ask number of points to insert
                int num_points;
                printf("Enter number of points to insert: ");
                scanf("%d", &num_points);
                for (int i = 0; i < num_points; i++) {
                    Point *p = (Point *)malloc(sizeof(Point));
                    if (!p) {
                        perror("Failed to allocate memory for point");
                        break;
                    }
                    printf("Enter point to insert (x y z): ");
                    scanf("%f %f %f", &p->x, &p->y, &p->z);
    
                    if (searchPoint(root, p) == NULL){
                        insertPoint(root, p);    
                        free(p); // Point is copied into the octree, free allocated memory
                    } else{
                        printf("Point (%.2f, %.2f, %.2f) already exists in the octree. Skipping duplicate.\n", p->x, p->y, p->z);
                        free(p); // Free memory for duplicate point
                    }
                    free(p);
                    printTree(root);
                }
                break;
            }
            case 'd': {
                Point *p = (Point *)malloc(sizeof(Point));
                if (!p) {
                    perror("Failed to allocate memory for point");
                    break;
                }
                printf("Enter point to delete (x y z): ");
                scanf("%f %f %f", &p->x, &p->y, &p->z);
                deletePoint(root, p);
                free(p);
                printTree(root);
                break;
            }
            case 's': {
                Point *p = (Point *)malloc(sizeof(Point));
                if (!p) {
                    perror("Failed to allocate memory for point");
                    break;
                }
                printf("Enter point to search (x y z): ");
                scanf("%f %f %f", &p->x, &p->y, &p->z);
                OctreeNode *node = searchPoint(root, p);
                if (node) {
                    printf("Point (%.2f, %.2f, %.2f) found in the octree.\n", p->x, p->y, p->z);
                } else {
                    printf("Point (%.2f, %.2f, %.2f) not found in the octree.\n", p->x, p->y, p->z);
                }
                free(p);
                printTree(root);
                break;
            }
            case 'r': {
                Point min, max;
                printf("Enter the minimum corner of the cube (x y z): ");
                scanf("%f %f %f", &min.x, &min.y, &min.z);
                printf("Enter the maximum corner of the cube (x y z): ");
                scanf("%f %f %f", &max.x, &max.y, &max.z);
                int count = 0;
                rangeQuery(root, &min, &max, &count, fp);
                printf("Total points within the cube: %d\n", count);
                printTree(root);
                break;
            }
            case 'n': {
                Point target, nearest;
                float minDist = MAX_SIZE * MAX_SIZE * 3;  // Initialize to maximum possible distance
                printf("Enter target point (x y z): ");
                scanf("%f %f %f", &target.x, &target.y, &target.z);
                if (findNearestNeighbor(root, target, &nearest, &minDist)) {
                    printf("Nearest neighbor to (%.2f, %.2f, %.2f) is (%.2f, %.2f, %.2f) with distance %.2f\n",
                           target.x, target.y, target.z, nearest.x, nearest.y, nearest.z, minDist);
                } else {
                    printf("No points found in the octree.\n");
                }
                printTree(root);
                break;
            }
            case 'q': return 0;
            default: printf("Invalid command.\n"); continue;
        }
    }
    fclose(fp);

}