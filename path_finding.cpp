
using namespace std;
#include <vector>
#include <iostream>


int path(const vector<vector<int>>& matrix, int CurrentNode, int Destination){
    if (matrix[CurrentNode][Destination] > 0){
        cout << "path is" << CurrentNode << " -> " << Destination << endl;
        return 1;
    }
    else if(matrix[CurrentNode][Destination] < 0){
        cout << "path is" << CurrentNode << " -> " << Destination << endl;
        return -1;
    }
    else{
        for(int j=0; j<7;j++){
           cout << "For loop 1" << endl;				//debugging line
           if (matrix[CurrentNode][j] != 0){
                cout << "If loop 1"<< endl;			//debugging line
                if (matrix[j][Destination] > 0){
                    cout << "If loop 2" <<endl;			//debugging line
                    cout << "Path is " << CurrentNode << " -> " << j << " -> "<<Destination << endl;
                    return 1;
                }
                else if(matrix[j][Destination] < 0){
                    cout << "else If loop 1" <<endl;			//debugging line
                    cout << "Path is " << CurrentNode << " -> " << j << " -> "<<Destination << endl;
                    return -1;

                }
                else{
                    for(int k=0; k<7;k++){
                        cout << "For loop 2" << endl;			//debugging line
                        if (matrix[j][k] != 0 && k != CurrentNode){
                            cout << "If loop 2"<< endl;			//debugging line
                            if (matrix[k][Destination] > 0){
                                cout << "Path is" << CurrentNode << " -> " << j << " -> " << k << " -> " << Destination << endl;
                                return 1;
                            }
                            else if (matrix[k][Destination] < 0){
                                cout << "Path is" << CurrentNode << " -> " << j << " -> " << k << " -> " << Destination << endl;
                                return -1;
                            }
                        }
                   }
                }
            }
        }
    }
}



int main()
{
    vector<vector<int>> matrix = {{ 0, 0, 0, 0, 3, -2, 0 },
                                  { 0, 0, 0, 0, 0, 1, -1 },
                                  { 0, 0, 0, 3, 0, -2, 0 },
                                  { 0, 0, -6, 0, 0, 0, 5 },
                                  { 6, 0, 0, 0, 0, 0, -5 },
                                  { -2, 1, 2, 0, 0, 0, 0 },
                                  { 0, 1, 0, -5, 5, 0, 0 }};

    path(matrix,0,3);
}

