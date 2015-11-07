#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;


struct matrix_data_type
{
    int columns;
    int rows;
    int nnz;

    vector <int> col_ind;
    vector <int> val;
    vector <int> row_ptr;

};

struct row_similarities
{
    int row;
    double similarity;
};

struct non_zero_value
{
    int value;
    int col_ind;
};

bool compareSimilarity(const row_similarities &a, const row_similarities &b)
{
    return a.similarity > b.similarity;
}

vector<non_zero_value> get_Vector(int k, matrix_data_type mdt)
{
    vector<non_zero_value> k_vector;
    int k_nnz = mdt.row_ptr[k+1] - mdt.row_ptr[k];
    if(!(k_nnz == 0))
    {
        for (int i = mdt.row_ptr[k]; i < (mdt.row_ptr[k] + k_nnz); i++)
        {
            non_zero_value nnz;
            nnz.col_ind = mdt.col_ind[i];
            nnz.value = mdt.val[i];
            k_vector.push_back(nnz);
        }
    }
    else
    {
        //cout << "This vector is empty";
    }

    return k_vector;
}

int cosine_similarity(matrix_data_type mdt, int k)
{
    vector<non_zero_value> k_vector = get_Vector(k, mdt);
    vector<row_similarities> row_sims;


    //making sure the chosen vector has non-zero values
    if (k_vector.size() > 0)
    {
        //for each row in the matrix...
        for (int j = 1; j <= mdt.rows; j++)
        {
            double cosine_sim = 0;
            //...that is not the chosen matrix...
            if(j != k)
            {
                //...return the vector
                vector<non_zero_value> j_vector = get_Vector(j, mdt);


                //compare each item in k to each item in j. create a cs for ones with matching col_ind.
                for(int m = 0; m < k_vector.size(); m++)
                {
                    for(int n = 0; n < j_vector.size(); n++)
                    {
                        non_zero_value k = k_vector[m];
                        non_zero_value j = j_vector[n];
                        if(k.col_ind == j.col_ind)
                        {
                            cosine_sim = cosine_sim + (j_vector[n].value * k_vector[m].value);
                        }
                    }
                }
                int length = j_vector.size() * k_vector.size();
                row_similarities rs;
                rs.row = j;
                if(length > 0)
                {
                    rs.similarity = (cosine_sim/length);
                }
                else
                {
                    rs.similarity = 0;
                }
                row_sims.push_back(rs);
            }
        }
    }

    std::sort(row_sims.begin(), row_sims.end(), compareSimilarity);
    for (int i = 0; i < row_sims.size(); i++)
    {
        cout << "Item: " <<row_sims[i].row << " -- Cosine Value: " << row_sims[i].similarity << endl;
    }



    return 0;
}

matrix_data_type readFile(matrix_data_type mdt)
{

    cout << "Please input the path to your file." << endl;
    string path;
    //cin >> path;
    path = "C:\\Users\\Scott\\Desktop\\pa1_data2\\test4.txt";
    string line;
    ifstream myfile(path.c_str());
    int line_count = 0;

    clock_t start2 = clock();

    if (myfile.is_open())
    {
        while(getline (myfile,line))
        {
            //For first line of file
            if(line_count < 1)
            {
                string buf;
                stringstream ss(line);
                vector<string> tokens;
                while (ss >> buf)
                {
                    tokens.push_back(buf);
                }
                string columns_str = tokens[1];
                string rows_str = tokens[0];
                string nnz_str = tokens[2];
                mdt.columns = atoi(columns_str.c_str());
                mdt.rows =  atoi(rows_str.c_str());
                mdt.nnz = atoi(nnz_str.c_str());

                for(int i=0; i < (mdt.rows + 1); i++)
                {
                    mdt.row_ptr.push_back(0);
                }
                line_count++;
            }
            else
            {
                if(line.length() > 0)
                {
                    string buf;
                    stringstream ss(line);
                    vector<string> tokens;
                    while (ss >> buf)
                    {
                        tokens.push_back(buf);
                    }
                    for (int i = 0; i < (tokens.size() - 1); ++i)
                    {
                        mdt.col_ind.push_back(atoi(tokens[i].c_str()) - 1);
                        mdt.val.push_back(1);
                        mdt.row_ptr[line_count]++;
                        i++;
                    }
                }
                line_count++;
            }
        }
    }
    else
    {
        cout << "Unable to open file";
    }

    //accumulate row_ptr

    for(int i=1; i <= mdt.row_ptr.size(); i++)
    {
        mdt.row_ptr[i] = mdt.row_ptr[i] + mdt.row_ptr[i-1];
    }


    myfile.close();

    clock_t end2 = clock();
    double time2 = (double) (end2-start2) / CLOCKS_PER_SEC * 1000.0;

    cout << "The import process took: " << time2 << " milliseconds" << endl;

    return mdt;
}

int print_mdt(matrix_data_type mdt, string path)
{
    ofstream myfile;
    myfile.open (path.c_str());
    //myfile << "Writing this to a file.\n";
    if(myfile.is_open())
    {
        myfile << "Col_ind: ";
        for(int i=0; i < mdt.col_ind.size(); i++)
        {
            myfile << "|" << mdt.col_ind[i] << "|";
            //cout << mdt.col_ind[i] << endl;
        }
        myfile << endl;
        myfile << "Col Array Size: " << mdt.col_ind.size() << endl;

        myfile << "Val:     ";
        for (int i=0; i < mdt.val.size(); i++)
        {
            myfile << "|" << mdt.val[i] << "|";
        }
        myfile << endl;
        myfile << "Val Array Size: " << mdt.val.size() << endl;

        myfile << "Row Ptr: ";
        for (int i =0; i < mdt.row_ptr.size(); i++)
        {
            myfile << "|" << mdt.row_ptr[i] << "|";
        }
        myfile << endl;
        myfile << "Row Ptr Array Size: " << mdt.row_ptr.size() << endl;
    }
    else
    {
        cout << "Could not open file to write to.";
    }
    myfile.close();
    return 0;
}

string run_menu(matrix_data_type mdt)
{
    cout << "Please enter the index for the item you wish to get similarities for:" << endl;
    string menu_choice;
    cin >> menu_choice;

    int menu_choice_int = atoi(menu_choice.c_str());

    if( menu_choice_int > mdt.rows)
    {
        menu_choice = "0";
        menu_choice_int = 0;
        cout << "The number you chose is not an item index. Please select a number between 0 and " << mdt.rows << "." << endl;
    }
    else
    {
        cout << "You chose item " << menu_choice << "." << endl;
    }

    return menu_choice;
}

void transpose_matrix(matrix_data_type &mdt, matrix_data_type &t_mdt)
{
    t_mdt.rows = mdt.columns;
    t_mdt.columns = mdt.rows;
    t_mdt.nnz = mdt.nnz;
    vector<int> row_counts;

    for (int i = 0; i < (t_mdt.rows + 1); i++)
    {
        t_mdt.row_ptr.push_back(0);
    }

    for(int i = 0; i <t_mdt.rows; i++)
    {
        row_counts.push_back(0);
    }

    for(int i = 0; i < t_mdt.nnz; i++)
    {
        t_mdt.col_ind.push_back(0);
    }

    for(int i = 0; i < t_mdt.nnz; i++)
    {
        t_mdt.val.push_back(1);
    }

    for (int i = 0; i < mdt.rows; i++)
    {
        for (int j = mdt.row_ptr[i]; j < mdt.row_ptr[i+1]; j++)
        {
            int i2 = mdt.col_ind[j];
            t_mdt.row_ptr[i2+1]++;
        }
    }

    for (int i = 1; i <= t_mdt.row_ptr.size(); i++)
    {
        t_mdt.row_ptr[i] = t_mdt.row_ptr[i] + t_mdt.row_ptr[i-1];
    }

    //for (int i = 0; i <= t_mdt.row_ptr.size(); i++)
    //{
        //t_mdt.row_ptr[i]++;
    //}

    for(int i = 0; i < mdt.rows; i++)
    {
        for(int j = mdt.row_ptr[i]; j < mdt.row_ptr[i+1]; j++)
        {
            int i2 = mdt.col_ind[j];
            int x = t_mdt.row_ptr[i2] + row_counts[i2];
            t_mdt.col_ind[x] = i;
            t_mdt.val[t_mdt.row_ptr[i2] + row_counts[i2]] = mdt.val[j];
            row_counts[i2]++;
        }
    }

    cout << "A transpose of the matrix has been saved in memory." << endl;
}

//vector<row_similarities> top_n()

int main()
{
    cout << "This program calculates the cosine similarity of any two rows or columns of a given matrix." << endl;
    cout << "*******************************************************************************************" << endl;
    cout << endl;
    cout << endl;

    matrix_data_type mdt;

    mdt = readFile(mdt);
    cout << "Your file has been read and the matrix is now in memory." << endl;
    string path = "C:\\Users\\Scott\\Desktop\\pa1_data2\\output1.txt";
    print_mdt(mdt, path);

    //create the transposed matrix data type
    matrix_data_type t_mdt;
    transpose_matrix(mdt, t_mdt);
    string transpose_path = "C:\\Users\\Scott\\Desktop\\pa1_data2\\output2.txt";
    print_mdt(t_mdt, transpose_path);

    cout << "**************************************************************************************" << endl;

    string menu_selection;
    string top_n;
    menu_selection = run_menu(t_mdt);
    //top_n = get_top_n(top_n);
    int k = atoi(menu_selection.c_str());
    int l = atoi(top_n.c_str());
    cosine_similarity(t_mdt, k);


    string exit;
    cin >> exit;
    return 0;
}
