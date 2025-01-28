// C++ program to demonstrate Adjacency Matrix
// representation of undirected and unweighted graph
using namespace std;
#include <vector>
#include <iostream>


void path(const vector<vector<int>>& matrix, int CurrentNode, int Destination){
    if (matrix[CurrentNode][Destination] == 1){
        cout << "path is" << CurrentNode << " -> " << Destination << endl;
        return;
    }
    else{
        for(int j=0; j<7;j++){
            cout << "For loop 1" << endl;
            if (matrix[CurrentNode][j] == 1){
                cout << "If loop 1"<< endl;
                if (matrix[j][Destination] == 1){
                    cout << "If loop 2" <<endl;
                    cout << "Path is " << CurrentNode << " -> " << j << " ->"<<Destination << endl;
                    return;
                }
                else{
                    for(int k=0; k<7;k++){
                        cout << "For loop 2" << endl;
                        if (matrix[j][k] == 1 && k != CurrentNode){
                            cout << "If loop 2"<< endl;
                            if (matrix[k][Destination] == 1)
                                cout << "path is" << CurrentNode << " -> " << j << " -> " << k << " -> " << Destination << endl;
                                return;
                        }

                    }

                }
            }
        }
    }
}

int main()
{
    vector<vector<int>> matrix = {{ 0, 0, 0, 0, 1, 1, 0 },
                                  { 0, 0, 0, 0, 0, 1, 1 },
                                  { 0, 0, 0, 1, 0, 1, 0 },
                                  { 0, 0, 1, 0, 0, 0, 1 },
                                  { 1, 0, 0, 0, 0, 0, 1 },
                                  { 1, 1, 1, 0, 0, 0, 0 },
                                  { 0, 1, 0, 1, 1, 0, 0 }};

    path(matrix,0,2);
}

