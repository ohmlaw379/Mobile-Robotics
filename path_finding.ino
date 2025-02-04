//#include <Arduino.h>
#include <vector>

const int INF = 1e9; // Define infinity as a large value
const int V = 5; // Number of vertices

int distanceSensorPin = 16; // define distance sensor pin
bool obstacleDetected = false; // bool value for obstacle detection


std::vector<std::vector<int>> adjMatrix = {
    {0, 4, 6, INF, 2},
    {4, 0, 2, 5, 5},
    {6, 4, 0, 2, INF},
    {INF, 5, 2, 0, 8},
    {2, 5, INF, 8, 0}
};

std::vector<int> dijkstra(int start, int destination) {
    std::vector<int> dist(V, INF);
    std::vector<int> parent(V, -1);
    std::vector<bool> visited(V, false);
    dist[start] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = -1;
        for (int i = 0; i < V; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (u == -1 || dist[u] == INF) break;

        visited[u] = true;
        for (int v = 0; v < V; v++) {
            if (!visited[v] && adjMatrix[u][v] != INF) {
                int newDist = dist[u] + adjMatrix[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u;
                }
            }
        }
    }

    std::vector<int> path;
    for (int v = destination; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    return path;
}

void getPath(std::vector<int> route){
  for(int i=0; i<route.size()-1; i++){
    dijkstra(route[i],route[i+1]);
  }
}

void setup() {
    Serial.begin(9600);
    while (!Serial); // Wait for serial monitor to open

    // Get route from server e.g {0 2 4 3}

    int startNode = 0;
    int destinationNode = 3;
    std::vector<int> path = dijkstra(startNode, destinationNode);

    Serial.print("Shortest path from node ");
    Serial.print(startNode);
    Serial.print(" to node ");
    Serial.print(destinationNode);
    Serial.print(": ");
    for (int node : path) {
        Serial.print(node);
        Serial.print(" ");
    }
    Serial.println();
    
}


void loop() {
  int distanceValue = analogRead(distanceSensorPin);
  int threshold = 3000;
  if (distanceValue > threshold)
    obstacleDetected == true;


  if(obstacleDetected == true){
    
  }
    // stop the car
    // rotate 180 degrees
    // car moves to closest junction
    // chnage the adjaceny matrix
    // car goes through new path
  
  // call getPath function with the route as parameters


}
