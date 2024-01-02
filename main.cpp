#include<bits/stdc++.h>

using namespace std;

int trainingData = 150;
int testingData = 50;
int no_of_parameters = 3;

vector<vector<long double>> find_transpose(vector<vector<long double>> &matrix)
{
    int n = matrix.size();
    int m = matrix[0].size();

    vector<vector<long double>> transpose_matrix(m,vector<long double>(n));

    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            transpose_matrix[i][j] = matrix[j][i];
        }
    }

    return transpose_matrix;
}

vector<vector<long double>> get_matrix(int x,int y,vector<vector<long double>> &matrix,int dimension)
{
    vector<vector<long double>> reduced_matrix(dimension-1,vector<long double>(dimension-1));
    int p = 0;
    int q = 0;

    for(int i=0;i<dimension;i++)
    {
        for(int j=0;j<dimension;j++)
        {
            if(i == x || j == y) continue;
            reduced_matrix[p][q] = matrix[i][j];
            q++;
            if(q == dimension - 1)
            {
                q=0;
                p++;
            }

        }
    }
    return reduced_matrix;


}

long double find_determinant(vector<vector<long double>> &matrix,int dimension)
{
    if(dimension == 1)
    {
        return matrix[0][0];
    }
    int sign = 1;
    long double sum = 0;
    for(int i=0;i<dimension;i++)
    {
        vector<vector<long double>> reduced_matrix = get_matrix(0,i,matrix,dimension);
        sum += (sign * matrix[0][i] * find_determinant(reduced_matrix,dimension-1));
        sign = sign*(-1);
    }
    return sum;

}

vector<vector<long double>> find_adjoint(vector<vector<long double>> &matrix,int dimension)
{
    vector<vector<long double>> adjoint_matrix(dimension,vector<long double>(dimension));
    for(int i=0;i<dimension;i++)
    {
        for(int j=0;j<dimension;j++)
        {
            vector<vector<long double>> reduced_matrix = get_matrix(i,j,matrix,dimension);
            adjoint_matrix[i][j] = pow(-1,i+j)*find_determinant(reduced_matrix,dimension-1);

        }
    }

    return adjoint_matrix;

}

vector<vector<long double>> find_inverse(vector<vector<long double>> &matrix,int dimension)
{
    vector<vector<long double>> inverse = find_adjoint(matrix,dimension);
    long double determinant = find_determinant(matrix,dimension);

    for(int i=0;i<dimension;i++)
    {
        for(int j=0;j<dimension;j++)
        {
            inverse[i][j] = inverse[i][j]/determinant;
        }
    }

    return find_transpose(inverse);
}



vector<vector<long double>> multiple_matrix(vector<vector<long double>> &matrix1,vector<vector<long double>> &matrix2)
{
    int n = matrix1.size();
    int m = matrix2[0].size();
    vector<vector<long double>> final_matrix(n,vector<long double>(m));

    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            long double val = 0;
            for(int k=0;k<matrix1[0].size();k++)
            {
                val += (matrix1[i][k])*(matrix2[k][j]);
            }
            final_matrix[i][j] = val;
        }
    }

    return final_matrix;

}

void print_matrix(vector<vector<long double>> &matrix1)
{
    int m=matrix1.size();
    int n=matrix1[0].size();
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            cout << matrix1[i][j];
            cout << " ";
        }
        cout << endl;
    }
}

void read_csv_training(vector<vector<long double>> &x,vector<vector<long double>> &y,string filename)
{
    ifstream file(filename);
    string line;
    int index = 0;
    while (getline(file, line)) {
        vector<string> row;
        stringstream lineStream(line);
        string cell;

        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }
        x[index][0] = 1;
        x[index][1] = stod(row[0]);
        x[index][2] = stod(row[1]);
        x[index][3] = stod(row[2]);
        y[index][0] = stod(row[3]);
        index++;
    }

    file.close();


}





int main(){

    vector<vector<long double>> x(trainingData,vector<long double>(no_of_parameters + 1));
    vector<vector<long double>> y(trainingData,vector<long double>(1));
    vector<vector<long double>> xtest(testingData,vector<long double>(no_of_parameters+1));
    vector<vector<long double>> ytest(testingData,vector<long double>(1));
    read_csv_training(x,y,"training_dataset.csv");
    read_csv_training(xtest,ytest,"testing_dataset.csv");
    vector<vector<long double>> x_t=find_transpose(x);
    vector<vector<long double>> x1=multiple_matrix(x_t,x);
    long double alpha=0.5;
    for(int i=0;i<no_of_parameters;i++)
    {
        x1[i][i]+=alpha;
    }
    vector<vector<long double>> x2=find_inverse(x1,no_of_parameters + 1);
    vector<vector<long double>> x3=multiple_matrix(x2,x_t);
    vector<vector<long double>> w=multiple_matrix(x3,y);;
    vector<vector<long double>> y_dash=multiple_matrix(x,w);
    vector<vector<long double>> y_dasht=multiple_matrix(xtest,w);
    vector<vector<long double>> e=y;
    for(int i=0;i<trainingData;i++)
    {
        e[i][0]-=y_dash[i][0];
    }

    long double mu=0;

    for(int i=0;i<trainingData;i++)
    {
        mu+=e[i][0];
    }

    long double rmse = 0;
    for(int i=0;i<testingData;i++)
    {
        rmse+=((y_dasht[i][0]-ytest[i][0])*(y_dasht[i][0]-ytest[i][0]));
    }
    rmse/=testingData;
    rmse=sqrt(rmse);

    mu/=trainingData;
    long double sigma_2=0;
    for(int i=0;i<trainingData;i++)
    {
        sigma_2+=((e[i][0]-mu)*(e[i][0]-mu));
    }
    sigma_2/=trainingData;
    long double beta=1/sigma_2;

    cout<<"Predicted Output"<<" "<< "Actual Output"<<endl;
    for(int i=0;i<testingData;i++)
    {
        cout << y_dasht[i][0]<< "\t\t\t\t" <<ytest[i][0]<<endl;
    }
    cout <<"Beta(PRECISION OF ERROR TERM): "<< beta << endl;
    cout << "RMSE(ROOT MEAN SQUARE ERROR) : " << rmse << endl; 




}