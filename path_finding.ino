#include <vector>

int CurrentNode = 1;
int Destination = 2;
std::vector<std::vector<int>> matrix = {{ 0, 0, 0, 0, 1, 1, 0 },
                              { 0, 0, 0, 0, 0, 1, 1 },
                              { 0, 0, 0, 1, 0, 1, 0 },
                              { 0, 0, 1, 0, 0, 0, 1 },
                              { 1, 0, 0, 0, 0, 0, 1 },
                              { 1, 1, 1, 0, 0, 0, 0 },
                              { 0, 1, 0, 1, 1, 0, 0 }};


void setup(){
  Serial.begin(9600);

}

std::vector<int> path(const std::vector<std::vector<int>>& matrix, int CurrentNode, int Destination){
    int Node1;
    std::vector<int> route;
    if (matrix[CurrentNode][Destination] != 0){
        route = {CurrentNode,Destination};
        return route;
    }
    else{
        if(CurrentNode == 4 || CurrentNode == 3){
           for(int j=0; j<7;j++){
              if (matrix[CurrentNode][j] != 0){
                if (matrix[j][Destination] != 0){
                    if (j != 5 && j != 6){
                        route = {CurrentNode, j ,Destination};
                        return route;
                    }
                    else{
                        return route = {CurrentNode,Destination};
                    }
                }
                else{
                    for(int k=0; k<7;k++){
                        if (matrix[j][k] != 0 && k != CurrentNode){
                            if (matrix[k][Destination] > 0){
                                if ((k != 5 && k != 6) && (j !=5 && j !=6)){
                                    route = {CurrentNode, j, k ,Destination};
                                    return route;
                                }
                                else if((j !=5 && j !=6) && (k == 5 && k==6)){
                                    route = {CurrentNode,j, Destination };
                                }
                                else if((k !=5 && k !=6) && (j == 5 || j == 6)){
                                    route = {CurrentNode, k ,Destination};
                                    return route;
                                }
                            }
                        }
                   }
                }
            }
        }
    }
        if((CurrentNode == 1 && Destination == 0)||(CurrentNode == 1 && Destination == 2)){
           for(int j=0; j<7;j++){
              if (matrix[CurrentNode][j] != 0){
                if (matrix[j][Destination] != 0){
                    if (j != 5 && j != 6){
                        route = {CurrentNode, j ,Destination};
                        return route;
                    }
                    else{
                        return route = {CurrentNode,Destination};
                    }
                }
                else{
                    for(int k=0; k<7;k++){
                        if (matrix[j][k] != 0 && k != CurrentNode){
                            if (matrix[k][Destination] > 0){
                                if ((k != 5 && k != 6) && (j !=5 && j !=6)){
                                    route = {CurrentNode, j, k ,Destination};
                                    return route;
                                }
                                else if((j !=5 && j !=6) && (k == 5 && k==6)){
                                    route = {CurrentNode,j, Destination };
                                }
                                else if((k !=5 && k !=6) && (j == 5 || j == 6)){
                                    route = {CurrentNode, k ,Destination};
                                    return route;
                                }
                            }
                        }
                   }
                }
            }
        }
    }
        for(int j=7; j>0;j--){
           if (matrix[CurrentNode][j] != 0){
                if (matrix[j][Destination] > 0){
                    if (j != 5 && j != 6){
                        route = {CurrentNode, j ,Destination};
                        return route;
                    }
                    else{
                        return route = {CurrentNode,Destination};
                    }
                }
                else{
                    for(int k=0; k<7;k++){
                        if (matrix[j][k] != 0 && k != CurrentNode){
                            if (matrix[k][Destination] > 0){
                                if ((k != 5 && k != 6) && (j !=5 && j!=6)){
                                    route = {CurrentNode, j ,k ,Destination};
                                    return route;
                                }
                                else if((j !=5 && j !=6) && (k == 5 && k==6)){
                                    route = {CurrentNode,j, Destination };
                                }
                                else if((k !=5 && k !=6) && (j == 5 || j == 6)){
                                    route = {CurrentNode, k ,Destination};
                                    return route;
                                }
                            }
                        }
                   }
                }
            }
        }
    }
}

void loop(){
  std::vector<int> PrintPath = path(matrix,CurrentNode,Destination);
  for (int i = 0; i < PrintPath.size(); i++) {
    Serial.println(PrintPath[i]);
    if (i < PrintPath.size() - 1)  Serial.println(" -> ");  // Print arrows between nodes
  }
  Serial.println("\n");
}




