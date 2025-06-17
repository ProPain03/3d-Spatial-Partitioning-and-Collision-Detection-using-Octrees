const apiClient = {
    baseUrl: 'http://localhost:5000/api',

    async insertPoint(point) {
        const response = await fetch(`${this.baseUrl}/octree/insert`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(point),
        });
        return response.json();
    },

    async deletePoint(point) {
        const response = await fetch(`${this.baseUrl}/octree/delete`, {
            method: 'DELETE',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(point),
        });
        return response.json();
    },

    async searchPoint(point) {
        const response = await fetch(`${this.baseUrl}/octree/search`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(point),
        });
        return response.json();
    },

    async rangeQuery(min, max) {
        const response = await fetch(`${this.baseUrl}/octree/range`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ min, max }),
        });
        return response.json();
    },

    async nearestNeighbor(point) {
        const response = await fetch(`${this.baseUrl}/octree/nearest`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(point),
        });
        return response.json();
    },

    async movePoint(oldPoint, newPoint) {
        const response = await fetch(`${this.baseUrl}/game/move`, {
            method: 'PUT',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ oldPoint, newPoint }),
        });
        return response.json();
    },

    async movePointByDirection(point, direction) {
        const response = await fetch(`${this.baseUrl}/game/move_direction`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ point, direction }),
        });
        return response.json();
    },

    async detectCollision(point) {
        const response = await fetch(`${this.baseUrl}/game/collision`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(point),
        });
        return response.json();
    },

    async getAllPoints() {
        const response = await fetch(`${this.baseUrl}/octree/points`);
        return response.json();
    },

    async getOctreeStructure() {
        const response = await fetch(`${this.baseUrl}/octree/structure`);
        return response.json();
    }
};

// Make it globally available
window.apiClient = apiClient;