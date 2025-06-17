class PointManager {
    constructor() {
        this.points = [];
        this.selectedPoint = null;
    }

    async loadPoints() {
        try {
            const response = await fetch('/api/octree/points');
            const data = await response.json();
            
            if (data.success) {
                this.points = data.points;
                return this.points;
            } else {
                console.error('Failed to load points:', data.message);
                return [];
            }
        } catch (error) {
            console.error('Error loading points:', error);
            return [];
        }
    }

    async addPoint(point) {
        try {
            const response = await fetch('/api/octree/insert', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(point)
            });
            
            const data = await response.json();
            
            if (data.success) {
                this.points.push(point);
                return true;
            } else {
                console.error('Failed to add point:', data.message);
                return false;
            }
        } catch (error) {
            console.error('Error adding point:', error);
            return false;
        }
    }

    async removePoint(point) {
        try {
            const response = await fetch('/api/octree/delete', {
                method: 'DELETE',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(point)
            });
            
            const data = await response.json();
            
            if (data.success) {
                this.points = this.points.filter(p => 
                    p.x !== point.x || p.y !== point.y || p.z !== point.z
                );
                return true;
            } else {
                console.error('Failed to remove point:', data.message);
                return false;
            }
        } catch (error) {
            console.error('Error removing point:', error);
            return false;
        }
    }

    async searchPoint(point) {
        try {
            const response = await fetch('/api/octree/search', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(point)
            });
            
            const data = await response.json();
            return data;
        } catch (error) {
            console.error('Error searching point:', error);
            return { success: false, found: false };
        }
    }

    async rangeQuery(minPoint, maxPoint) {
        try {
            const response = await fetch('/api/octree/range', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ min: minPoint, max: maxPoint })
            });
            
            const data = await response.json();
            return data;
        } catch (error) {
            console.error('Error in range query:', error);
            return { success: false, points: [] };
        }
    }

    async findNearestNeighbor(targetPoint) {
        try {
            const response = await fetch('/api/octree/nearest', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(targetPoint)
            });
            
            const data = await response.json();
            return data;
        } catch (error) {
            console.error('Error finding nearest neighbor:', error);
            return { success: false };
        }
    }
}