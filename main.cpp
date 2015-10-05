#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <cstdio>
#include <ctime>

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




matrix_data_type readFile(matrix_data_type mdt)
{

    cout << "Please input the path to your file." << endl;
    string path;
    //cin >> path;
    path = "C:\\Users\\Scott\\Desktop\\pa1_data2\\test1.txt";
    string line;
    ifstream myfile(path.c_str());
    int line_count = 0;

    clock_t start2 = clock();

    if (myfile.is_open())
    {


        while(getline (myfile,line))
        {
            if(line_count < 1)
            {
                string buf;
                stringstream ss(line);
                vector<string> tokens;
                while (ss >> buf)
                {
                    tokens.push_back(buf);
                }
                //for (int i = 0; i < tokens.size(); ++i)
                //    cout << tokens[i] << endl;

                string columns_str = tokens[1];
                string rows_str = tokens[0];
                string nnz_str = tokens[2];
                mdt.columns = atoi(columns_str.c_str());
                mdt.rows =  atoi(rows_str.c_str());
                mdt.nnz = atoi(nnz_str.c_str());

                //cout << "Columns: " << mdt.columns << endl;
                //cout << "Rows: " << mdt.rows << endl;
                //cout << "NNZ: " << mdt.nnz << endl;

                for(int i=0;i<(mdt.rows + 1);i++)
                {
                    mdt.row_ptr.push_back(0);
                }

                line_count++;
            }
            else
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
                    //cout << "Row: " << line_count << "-- Value: " << tokens[i + 1] << "-- Column index: " << tokens[i] << endl;
                    //cout << atoi(tokens[i+1].c_str())<<endl;
                    mdt.col_ind.push_back(atoi(tokens[i].c_str()));
                    mdt.val.push_back(1);
                    mdt.row_ptr[line_count]++;
                    i++;
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
        cout << "You chose " << menu_choice << "." << endl;
    }





    return menu_choice;
}

matrix_data_type transpose_matrix(matrix_data_type mdt, matrix_data_type t_mdt)
{
    t_mdt.rows = mdt.columns;
    t_mdt.columns = mdt.rows;
    t_mdt.nnz = mdt.nnz;

    vector<int> row_counts;

    for(int i = 0; i < (t_mdt.rows + 1); i++ )
    {
        t_mdt.row_ptr.push_back(0);
    }

    for(int i = 0; i < (t_mdt.rows + 1); i++)
    {
            row_counts.push_back(0);
    }

    for(int i = 0; i < t_mdt.nnz; i++)
    {
        t_mdt.col_ind.push_back(0);
        t_mdt.val.push_back(0);
    }

    for(int i = 0; i < mdt.rows; i++)
    {
        for(int j = mdt.row_ptr[i]; j < mdt.row_ptr[i+1]; j++)
        {
            int i2 = mdt.col_ind[j];
            t_mdt.row_ptr[i2+1] ++;
        }
    }

    //Construct the final row_ptr2 here
    for(int i=1; i <= t_mdt.row_ptr.size(); i++)
    {
        t_mdt.row_ptr[i] = t_mdt.row_ptr[i] + t_mdt.row_ptr[i-1];
    }


    //second run: fill out the new matrix
    for(int i = 0; i < mdt.rows; i++)
    {
        for(int j = mdt.row_ptr[i]; j < mdt.row_ptr[i+1]; j++)
        {
            int i2 = mdt.col_ind[j];
            t_mdt.col_ind[t_mdt.row_ptr[i2] + row_counts[i2]] = i;
            t_mdt.val[t_mdt.row_ptr[i2] + row_counts[i2]] = mdt.val[j];
            row_counts[i2]++;
        }
    }

    cout << "A transpose of the matrix has been saved in memory." << endl;

    return t_mdt;
}




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
    t_mdt = transpose_matrix(mdt, t_mdt);
    string transpose_path = "C:\\Users\\Scott\\Desktop\\pa1_data2\\output2.txt";
    print_mdt(t_mdt, transpose_path);

    cout << "**************************************************************************************" << endl;

    string menu_selection;
    menu_selection = run_menu(t_mdt);


    string exit;
    cin >> exit;
    return 0;
}
