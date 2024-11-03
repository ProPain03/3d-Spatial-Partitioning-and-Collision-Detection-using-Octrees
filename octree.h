// octree.h
#ifndef OCTREE_H
#define OCTREE_H

#include <stdbool.h>
#include <stdio.h>

// Define constants
#define MAX_SIZE 1000       // Maximum size of the octree
#define MAX_DEPTH 5        // Maximum depth of the octree
#define MAX_POINTS 2       // Maximum number of points in a node
#define STEP 50            // Step size for moving points
#define COLLISION_SIZE 30  // Size of the collision box around a moving point

// Point structure
typedef struct Point {
    float x, y, z;
} Point;

// Octree node structure
typedef struct OctreeNode {
    Point center;
    Point min;
    Point max;
    int ptCount;
    float size;
    int depth;
    int isLeaf;
    Point points[MAX_POINTS];
    struct OctreeNode *children[8];
} OctreeNode;

// Function prototypes
OctreeNode *createNode(Point center, float size, int depth);
int getOctant(Point *center, Point *p);
void subdivideNode(OctreeNode *node);
OctreeNode *searchPoint(OctreeNode *node, Point *point);
bool insertPoint(OctreeNode *node, Point *point);
void deletePoint(OctreeNode *node, Point *point);
void updatePointInTree(OctreeNode *root, Point *oldPoint, Point *newPoint);
void readPoints(const char *filename, OctreeNode *root);
void printTree(OctreeNode *node);
void rangeQuery(OctreeNode *node, Point *min, Point *max, int *count, FILE *fp);
bool detect_collision(OctreeNode *octree, Point moving_point, float box_size);
void freeTree(OctreeNode *node);
void deletePoint_collision(OctreeNode *node, Point *point);
bool insertPoint_collision(OctreeNode *node, Point *point);
bool findNearestNeighbor(OctreeNode *node, Point target, Point *nearest, float *minDist);

#endif // OCTREE_H