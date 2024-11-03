// octree.c
#include "octree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

// Function implementations

// Create a new octree node
OctreeNode *createNode(Point center, float size, int depth) {
    OctreeNode *node = (OctreeNode *)malloc(sizeof(OctreeNode));
    if (!node) {
        perror("Failed to allocate memory for octree node");
        exit(EXIT_FAILURE);
    }
    node->center = center;
    Point min = {center.x - size, center.y - size, center.z - size};
    Point max = {center.x + size, center.y + size, center.z + size};
    node->min = min;
    node->max = max;
    node->ptCount = 0;
    node->size = size;
    node->depth = depth;
    node->isLeaf = 1;  // Initially, a node is considered a leaf
    for (int i = 0; i < 8; i++) node->children[i] = NULL;
    return node;
}

// Determine the octant for a given point
int getOctant(Point *center, Point *p) {
    int octant = 0;
    if (p->x >= center->x) octant |= 4;
    if (p->y >= center->y) octant |= 2;
    if (p->z >= center->z) octant |= 1;
    return octant;
}

// Subdivide a node into eight children
void subdivideNode(OctreeNode *node) {
    float halfSize = node->size / 2.0;
    Point basecenter = node->center;

    for (int i = 0; i < 8; i++) {
        Point newPos = {
            basecenter.x + ((i & 4) ? halfSize : -halfSize),
            basecenter.y + ((i & 2) ? halfSize : -halfSize),
            basecenter.z + ((i & 1) ? halfSize : -halfSize)
        };
        node->children[i] = createNode(newPos, halfSize, node->depth + 1);
    }
    node->isLeaf = 0;
}

// Search for a point in the octree
OctreeNode *searchPoint(OctreeNode *node, Point *point) {
    if (node == NULL) return NULL;

    if (node->isLeaf) {
        for (int i = 0; i < node->ptCount; i++) {
            if (node->points[i].x == point->x && node->points[i].y == point->y && node->points[i].z == point->z) {
                return node;
            }
        }
    } else {
        int octant = getOctant(&node->center, point);
        return searchPoint(node->children[octant], point);
    }
    return NULL;
}

// Insert a point into the octree
bool insertPoint(OctreeNode *node, Point *point) {
    if (node->isLeaf){
        if (node->ptCount < MAX_POINTS) {
            node->points[node->ptCount++] = *point;
            printf("Inserted point (%.2f, %.2f, %.2f) at depth %d\n", point->x, point->y, point->z, node->depth);
            return true;
        } else if (node->depth == MAX_DEPTH) {
            printf("Max depth reached. Point (%.2f, %.2f, %.2f) not inserted.\n", point->x, point->y, point->z);
            return false;
        } 
        else {
            // Subdivide and redistribute points
            subdivideNode(node);
            for (int i = 0; i < node->ptCount; i++) {
                int octant = getOctant(&node->center, &node->points[i]);
                if (!insertPoint(node->children[octant], &node->points[i])) {
                    printf("Failed to redistribute point during subdivision.\n");
                    return false;
                }
            }
            node->ptCount = 0;
            int octant = getOctant(&node->center, point);
            return insertPoint(node->children[octant], point);
        }
    } else {
        int octant = getOctant(&node->center, point);
        return insertPoint(node->children[octant], point);
    }
}

// Insert a point into the octree for collision detection we don't print the inserted point
bool insertPoint_collision(OctreeNode *node, Point *point) {
    if (node->isLeaf){
        if (node->ptCount < MAX_POINTS) {
            node->points[node->ptCount++] = *point;
            //printf("Inserted point (%.2f, %.2f, %.2f) at depth %d\n", point->x, point->y, point->z, node->depth);
            return true;
        } else if (node->depth == MAX_DEPTH) {
            printf("Max depth reached.\n");
            return false;
        } 
        else {
            // Subdivide and redistribute points
            subdivideNode(node);
            for (int i = 0; i < node->ptCount; i++) {
                int octant = getOctant(&node->center, &node->points[i]);
                if (!insertPoint(node->children[octant], &node->points[i])) {
                    printf("Failed to redistribute point during subdivision.\n");
                    return false;
                }
            }
            node->ptCount = 0;
            int octant = getOctant(&node->center, point);
            return insertPoint(node->children[octant], point);
        }
    } else {
        int octant = getOctant(&node->center, point);
        return insertPoint(node->children[octant], point);
    }
}


// Delete a point from the octree
void deletePoint(OctreeNode *node, Point *point) {
    if (node == NULL) return;

    if (node->isLeaf) {
        int found = -1;
        for (int i = 0; i < node->ptCount; i++) {
            if (node->points[i].x == point->x && node->points[i].y == point->y && node->points[i].z == point->z) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            // Shift points to fill the gap
            for (int j = found; j < node->ptCount - 1; j++) {
                node->points[j] = node->points[j + 1];
            }
            node->ptCount--;
            printf("Deleted point (%.2f, %.2f, %.2f)\n", point->x, point->y, point->z);
        }
    } else {
        int octant = getOctant(&node->center, point);
        deletePoint(node->children[octant], point);

        // After deletion, check if children can be merged
        int allLeaves = 1;
        int totalPoints = 0;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != NULL) {
                if (!node->children[i]->isLeaf) {
                    allLeaves = 0;
                    break;
                }
                totalPoints += node->children[i]->ptCount;
            }
        }

        if (allLeaves && totalPoints <= MAX_POINTS) {
            // Merge children into this node
            int count = 0;
            for (int i = 0; i < 8; i++) {
                if (node->children[i] != NULL) {
                    for (int j = 0; j < node->children[i]->ptCount; j++) {
                        if (count < MAX_POINTS) {
                            node->points[count++] = node->children[i]->points[j];
                        } else {
                            printf("Cannot merge: Exceeds MAX_POINTS\n");
                            break;
                        }
                    }
                    free(node->children[i]);
                    node->children[i] = NULL;
                }
            }
            node->ptCount = count;
            node->isLeaf = 1;
            printf("Merged the children into node at depth %d\n", node->depth);
        }
    }
}

// Delete a point from the octree for collision detection, we don't print the deleted point
void deletePoint_collision(OctreeNode *node, Point *point) {
    if (node == NULL) return;

    if (node->isLeaf) {
        int found = -1;
        for (int i = 0; i < node->ptCount; i++) {
            if (node->points[i].x == point->x && node->points[i].y == point->y && node->points[i].z == point->z) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            // Shift points to fill the gap
            for (int j = found; j < node->ptCount - 1; j++) {
                node->points[j] = node->points[j + 1];
            }
            node->ptCount--;
            //printf("Deleted point (%.2f, %.2f, %.2f)\n", point->x, point->y, point->z);
        }
    } else {
        int octant = getOctant(&node->center, point);
        deletePoint_collision(node->children[octant], point);

        // After deletion, check if children can be merged
        int allLeaves = 1;
        int totalPoints = 0;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != NULL) {
                if (!node->children[i]->isLeaf) {
                    allLeaves = 0;
                    break;
                }
                totalPoints += node->children[i]->ptCount;
            }
        }

        if (allLeaves && totalPoints <= MAX_POINTS) {
            // Merge children into this node
            int count = 0;
            for (int i = 0; i < 8; i++) {
                if (node->children[i] != NULL) {
                    for (int j = 0; j < node->children[i]->ptCount; j++) {
                        if (count < MAX_POINTS) {
                            node->points[count++] = node->children[i]->points[j];
                        } else {
                            printf("Cannot merge: Exceeds MAX_POINTS\n");
                            break;
                        }
                    }
                    free(node->children[i]);
                    node->children[i] = NULL;
                }
            }
            node->ptCount = count;
            node->isLeaf = 1;
            //printf("Merged children into node at depth %d\n", node->depth);
        }
    }
}


// Update a point's position in the octree
void updatePointInTree(OctreeNode *root, Point *oldPoint, Point *newPoint) {
    OctreeNode *node = searchPoint(root, oldPoint);
    if (node == NULL) {
        printf("Old point (%.2f, %.2f, %.2f) not found in the octree.\n", oldPoint->x, oldPoint->y, oldPoint->z);
        return;
    }

    // Determine if the new point is still within the current node's boundaries
    if (newPoint->x >= node->min.x && newPoint->x < node->max.x &&
        newPoint->y >= node->min.y && newPoint->y < node->max.y &&
        newPoint->z >= node->min.z && newPoint->z < node->max.z) {
        // Update the point in place
        int index = -1;
        for (int i = 0; i < node->ptCount; i++) {
            if (node->points[i].x == oldPoint->x && node->points[i].y == oldPoint->y && node->points[i].z == oldPoint->z) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            node->points[index] = *newPoint;
            printf("Updated point in place within the same node to (%.2f, %.2f, %.2f)\n", newPoint->x, newPoint->y, newPoint->z);
        } else {
            printf("Point index not found in the node.\n");
        }
    } else {
        // Attempt to delete the old point
        deletePoint(root, oldPoint);

        // Attempt to insert the new point
        bool success = insertPoint(root, newPoint);

        if (success) {
            printf("Updated point from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)\n",
                   oldPoint->x, oldPoint->y, oldPoint->z, newPoint->x, newPoint->y, newPoint->z);
        } else {
            // Insertion failed, revert to the old point
            insertPoint(root, oldPoint);
            printf("Failed to insert new point (%.2f, %.2f, %.2f). Reverting to old point.\n",
                   newPoint->x, newPoint->y, newPoint->z);
        }
    }
}

// Read points from a file and insert them into the octree
void readPoints(const char *filename, OctreeNode *root) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    float x, y, z;
    int lineNumber = 0;
    while (fscanf(file, "%f %f %f", &x, &y, &z) == 3) {
        lineNumber++;
        Point *p = (Point *)malloc(sizeof(Point));
        if (!p) {
            perror("Failed to allocate memory for point");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        p->x = x; p->y = y; p->z = z;
        if (searchPoint(root, p) == NULL){
            insertPoint(root, p);    
            free(p); // Point is copied into the octree, free allocated memory
        } else{
            printf("Point (%.2f, %.2f, %.2f) already exists in the octree. Skipping duplicate at line %d.\n", p->x, p->y, p->z, lineNumber);
            free(p); // Free memory for duplicate point
        }
    }

    // Handle potential reading errors
    if (!feof(file)) {
        fprintf(stderr, "Error reading file at line %d.\n", lineNumber + 1);
    }

    fclose(file);
}

// Recursive function to print the octree structure to a file
void printTreeRecursive(OctreeNode *node, int level, FILE *fp) {
    if (node) {
        for(int i=0;i<level;i++) fprintf(fp, "  ");
        if (node->isLeaf) {
            fprintf(fp, "Leaf Node at depth %d with %d points:\n", node->depth, node->ptCount);
            for (int i = 0; i < node->ptCount; i++) {
                for(int j=0; j<level+1; j++) fprintf(fp, "  ");
                fprintf(fp, "Point: (%.2f, %.2f, %.2f)\n", node->points[i].x, node->points[i].y, node->points[i].z);
            }
        } else {
            fprintf(fp, "Internal Node at depth %d\n", node->depth);
            for (int i = 0; i < 8; i++) {
                printTreeRecursive(node->children[i], level + 1, fp);
            }
        }
    }
}

// Function to print the octree structure to "Octree.txt"
void printTree(OctreeNode *node) {
    FILE *fp = fopen("Octree.txt", "w");
    if (!fp) {
        perror("Failed to open Octree.txt for writing");
        return;
    }
    printTreeRecursive(node, 0, fp);
    fclose(fp);
}

// Check if a point is within a cube (inclusive)
bool isPointInCube(Point *p, Point *min, Point *max) {
    return (p->x >= min->x && p->x <= max->x &&
            p->y >= min->y && p->y <= max->y &&
            p->z >= min->z && p->z <= max->z);
}

// Check if a point is within a cube (exclusive upper bounds)
bool isPointInCube_1(Point *p, Point *min, Point *max) {
    return (p->x >= min->x && p->x < max->x &&
            p->y >= min->y && p->y < max->y &&
            p->z >= min->z && p->z < max->z);
}

// Recursive range query
void rangeQuery(OctreeNode *node, Point *min, Point *max, int *count, FILE *fp) {
    if (node == NULL) return;

    // Check if the node is completely outside the cube
    if (node->max.x < min->x || node->min.x > max->x ||
        node->max.y < min->y || node->min.y > max->y ||
        node->max.z < min->z || node->min.z > max->z) {
        return;
    }

    // If the node is a leaf, check each point
    if (node->isLeaf) {
        for (int i = 0; i < node->ptCount; i++) {
            if (isPointInCube(&node->points[i], min, max)) {
                fprintf(fp, "Point within cube: (%.2f, %.2f, %.2f)\n", node->points[i].x, node->points[i].y, node->points[i].z);
                (*count)++;
            }
        }
    } else {
        // If the node is not a leaf, recursively check its children
        for (int i = 0; i < 8; i++) {
            rangeQuery(node->children[i], min, max, count, fp);
        }
    }
}

// Inline range query with printing to console
void inlineRangeQuery(OctreeNode *node, Point *min, Point *max, int *count) {
    if (node == NULL) return;

    // Check if the node is completely outside the cube
    if (node->max.x < min->x || node->min.x > max->x ||
        node->max.y < min->y || node->min.y > max->y ||
        node->max.z < min->z || node->min.z > max->z) {
        return;
    }

    // If the node is a leaf, check each point
    if (node->isLeaf) {
        for (int i = 0; i < node->ptCount; i++) {
            if (isPointInCube(&node->points[i], min, max)) {
                printf("Point within cube: (%.2f, %.2f, %.2f)\n", node->points[i].x, node->points[i].y, node->points[i].z);
                (*count)++;
            }
        }
    } else {
        // If the node is not a leaf, recursively check its children
        for (int i = 0; i < 8; i++) {
            inlineRangeQuery(node->children[i], min, max, count);
        }
    }
}

// Detect collision by checking nearby points
bool detect_collision(OctreeNode *octree, Point moving_point, float box_size) {
    insertPoint_collision(octree, &moving_point);
    // Define the collision box around the moving point
    Point min = { moving_point.x - box_size, moving_point.y - box_size, moving_point.z - box_size };
    Point max = { moving_point.x + box_size, moving_point.y + box_size, moving_point.z + box_size };

    int count = 0;    

    inlineRangeQuery(octree, &min, &max, &count);

    // Check if any points were found in the collision box (excluding the moving point itself)
    if (count > 1) { // Assuming the moving point is already in the octree
        printf("Collision detected\n");
        deletePoint_collision(octree, &moving_point);
        return true;  // Collision detected
    }
    deletePoint_collision(octree, &moving_point);
    return false;  // No collision
}

// Free all memory allocated for the octree
void freeTree(OctreeNode *node) {
    if (node) {
        if (!node->isLeaf) {
            for (int i = 0; i < 8; i++) {
                freeTree(node->children[i]);
            }
        }
        free(node);
    }
}

// Helper function to calculate squared distance between two points
static float squaredDistance(Point *a, Point *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx*dx + dy*dy + dz*dz;
}

// Helper function to calculate squared distance from a point to a cube
static float distanceToCubeSquared(Point *p, Point *min, Point *max) {
    float dx = 0.0f, dy = 0.0f, dz = 0.0f;

    if (p->x < min->x) dx = min->x - p->x;
    else if (p->x > max->x) dx = p->x - max->x;

    if (p->y < min->y) dy = min->y - p->y;
    else if (p->y > max->y) dy = p->y - max->y;

    if (p->z < min->z) dz = min->z - p->z;
    else if (p->z > max->z) dz = p->z - max->z;

    return dx*dx + dy*dy + dz*dz;
}

// Nearest Neighbor Search Helper Function
bool findNearestNeighborHelper(OctreeNode *node, Point target, Point *nearest, float *minDist) {
    if (node == NULL) return false;

    // Calculate the squared distance from target to the node's cube
    float distToCube = distanceToCubeSquared(&target, &node->min, &node->max);
    if (distToCube > (*minDist)) {
        // Current node's region is farther than the best distance found
        return false;
    }

    bool found = false;

    if (node->isLeaf) {
        for (int i = 0; i < node->ptCount; i++) {
            float dist = squaredDistance(&target, &node->points[i]);
            if (dist < *minDist && dist != 0.0f) { // Exclude the target point itself
                *minDist = dist;
                *nearest = node->points[i];
                found = true;
            }
        }
    } else {
        // Determine the order to traverse child nodes based on proximity to target
        int childOrder[8];
        for (int i = 0; i < 8; i++) {
            childOrder[i] = i;
        }

        // Simple heuristic: sort children by distance to target
        for (int i = 0; i < 8 - 1; i++) {
            for (int j = i + 1; j < 8; j++) {
                if (node->children[childOrder[j]] == NULL) continue;
                float distA = distanceToCubeSquared(&target, &node->children[childOrder[i]]->min, &node->children[childOrder[i]]->max);
                float distB = distanceToCubeSquared(&target, &node->children[childOrder[j]]->min, &node->children[childOrder[j]]->max);
                if (distA > distB) {
                    int temp = childOrder[i];
                    childOrder[i] = childOrder[j];
                    childOrder[j] = temp;
                }
            }
        }

        // Traverse children in the sorted order
        for (int i = 0; i < 8; i++) {
            int idx = childOrder[i];
            if (node->children[idx] != NULL) {
                bool childFound = findNearestNeighborHelper(node->children[idx], target, nearest, minDist);
                if (childFound) {
                    found = true;
                }
            }
        }
    }

    return found;
}

// Nearest Neighbor Search Function
bool findNearestNeighbor(OctreeNode *node, Point target, Point *nearest, float *minDist) {
    *minDist = FLT_MAX;
    bool found = findNearestNeighborHelper(node, target, nearest, minDist);
    if (found) {
        *minDist = sqrtf(*minDist); // Return the actual distance
    }
    return found;
}

