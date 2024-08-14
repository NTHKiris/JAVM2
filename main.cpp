#include <iostream>
#include <fstream>
#include "StackFrame.h"
#include <vector>
#include "errors.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;
ofstream solution;
/*
Run the testcase written in `filename`
@param filename name of the file
*/
void test(string filename) {
    StackFrame *sf = new StackFrame();
    try {
        sf->run(filename);
    }
    catch (exception& e) {
        cout << e.what();
    }
    delete sf;
}
int countFilesInDirectory(const std::string& path) {
    DIR* dirp = opendir(path.c_str());
    if (dirp == nullptr) {
        std::cerr << "Error opening directory " << path << std::endl;
        return -1; // Return -1 on error
    }

    int file_count = 0;
    struct dirent* dp;
    while ((dp = readdir(dirp)) != nullptr) {
        struct stat filestat;
        std::string full_path = path + "/" + dp->d_name;
        
        // Use stat to get information about the file
        if (stat(full_path.c_str(), &filestat) == 0) {
            if (S_ISREG(filestat.st_mode)) { // Check if it's a regular file
                ++file_count;
            }
        }
    }

    closedir(dirp);
    return file_count;
}

bool createDirectory(const std::string& path) {
    struct stat info;

    if (stat(path.c_str(), &info) != 0) { // Check if path does not exist
        // Attempt to create the directory
        if (mkdir(path.c_str()) != 0) {
            std::cerr << "Error creating directory: " << path << std::endl;
            return false;
        } else {
            return true;
        }
    } else if (info.st_mode & S_IFDIR) { // Check if it's a directory
        return true;
    } else {
        std::cerr << "Error: Path exists but is not a directory: " << path << std::endl;
        return false;
    }
}



/*
Main function
*/
void printTestFail(int i) {
    string file_solution_you = "solution_you/";
	string file_solution = "solution/";
    if (i < 10) {
        file_solution_you = "sol00" + to_string(i) + ".txt";
        file_solution += "sol00" + to_string(i) + ".txt";
    } else if (i < 100) {
        file_solution_you += "sol0" + to_string(i) + ".txt";
        file_solution += "sol0" + to_string(i) + ".txt";
    } else {
        file_solution_you += "sol" + to_string(i) + ".txt";
        file_solution += "sol" + to_string(i) + ".txt";
    }
	ifstream read_solution_you(file_solution_you);
	ifstream read_solution(file_solution);
	string s1, s2;
    int k = 1;
	while (read_solution_you >> s1 && read_solution >> s2)
	{
		if (s1 != s2)
		{
            cout << "Fail test " << i << ". Diference occurs in line " << k << " of solution file\n"; 
			break;
		}
		k++;
	}
	if (read_solution_you >> s1 || read_solution >> s2)
	{
        cout << "Fail test " << i << ". Diference occurs in line " << k << " of solution file\n"; 
	}
}

void comparefile(int start, int end)
{
	string folder_solution_you = "solution_you/";
	string folder_solution = "solution/";
	vector<int> result;
	for (int i = start; i < end; i++)
	{
        string file_solution_you(folder_solution_you);
        string file_solution(folder_solution);
        if (i < 10) {
            file_solution_you = folder_solution_you + "sol00" + to_string(i) + ".txt";
            file_solution = folder_solution + "sol00" + to_string(i) + ".txt";
        } else if (i < 100) {
            file_solution_you = folder_solution_you + "sol0" + to_string(i) + ".txt";
            file_solution = folder_solution + "sol0" + to_string(i) + ".txt";
        } else {
            file_solution_you = folder_solution_you + "sol" + to_string(i) + ".txt";
            file_solution = folder_solution + "sol" + to_string(i) + ".txt";
        }

		ifstream read_solution_you(file_solution_you);
		ifstream read_solution(file_solution);
		if (read_solution_you.fail() || read_solution.fail())
		{
			cout << "Error reading file\n"
				 << end;
			return;
		}
		string s1, s2;
		while (read_solution_you >> s1 && read_solution >> s2)
		{
			if (s1 != s2)
			{
				result.push_back(i);
				break;
			}
		}
		if (read_solution_you >> s1 || read_solution >> s2)
		{
			if (result.size() == 0 || result.size() > 0 && result[result.size() - 1] != i)
				result.push_back(i);
		}
	}

	if (result.size() == 0)
	{
        // if (start == end) 
        //     cout << "Success: test " << start << " to " << end << endl;
		cout << "Success: test " << start << " to " << end << endl;
	}
	else
	{
		cout << "percent success : " << (1 - result.size() * 1.0 / (end - start)) * 100 << "%" << endl;
		cout << "Fail : test [";
		for (int i = 0; i < result.size() - 1; i++)
		{
			cout << result[i] << ", ";
		}
		cout << result[result.size() - 1] << "]\n";
		printTestFail(result[0]);
	}
}

int main(int argc, char **argv) {
    // if (argc < 2)
    // {
    //     cout << "Missing arg\n";
    //     return 1;
    // }
    // test(argv[1]);
    cout << "Start program" << endl;
    string folder_input = "testcase/";
    string s;
    string folder_solution_you = "solution_you/";
    createDirectory("solution_you");
    if (argc < 2)
    {
        int start = 0;
        int end = countFilesInDirectory("testcase");
        string file_solution_you = folder_solution_you;
        string file_testcase = folder_input;
        for (int i = start; i < end; i++)
        {
            if (i < 10) {
                file_solution_you = folder_solution_you + "sol00" + to_string(i) + ".txt";
                file_testcase = folder_input + "test00" + to_string(i) + ".txt";
            } else if (i < 100) {
                file_solution_you = folder_solution_you + "sol0" + to_string(i) + ".txt";
                file_testcase = folder_input + "test0" + to_string(i) + ".txt";
            } else {
                file_solution_you = folder_solution_you + "sol" + to_string(i) + ".txt";
                file_testcase = folder_input + "test" + to_string(i) + ".txt";
            }
            solution.open(file_solution_you);
            std::streambuf *coutbuf = std::cout.rdbuf(); // Save old cout buffer
            std::streambuf *cerrbuf = std::cerr.rdbuf(); // Save old cerr buffer
            std::cout.rdbuf(solution.rdbuf()); // Redirect cout to output_file
            std::cerr.rdbuf(solution.rdbuf()); // Redirect cerr to output_filetest(file_testcase);
            test(file_testcase);
            std::cout.rdbuf(coutbuf);
            std::cerr.rdbuf(cerrbuf);solution.close();
        }
        cout << "\nOK: runs without errors\n"
                << endl;
        comparefile(start, end);
    }
    if (argc == 2) {
            string s = argv[1];
            string file_solution_you = folder_solution_you;
            string file_testcase = folder_input;
            if (stoi(s) < 10) {
                file_solution_you = folder_solution_you + "sol00" + s + ".txt";
                file_testcase = folder_input + "test00" + s + ".txt";
            } else if (stoi(s) < 100) {
                file_solution_you = folder_solution_you + "sol0" + s + ".txt";
                file_testcase = folder_input + "test0" + s + ".txt";
            } else {
                file_solution_you = folder_solution_you + "sol" + s + ".txt";
                file_testcase = folder_input + "test" + s + ".txt";
            }
            solution.open(file_solution_you);
            std::streambuf *coutbuf = std::cout.rdbuf(); // Save old cout buffer
            std::streambuf *cerrbuf = std::cerr.rdbuf(); // Save old cerr buffer
            std::cout.rdbuf(solution.rdbuf()); // Redirect cout to output_file
            std::cerr.rdbuf(solution.rdbuf()); // Redirect cerr to output_filetest(file_testcase);
            test(file_testcase);
            std::cout.rdbuf(coutbuf);
            std::cerr.rdbuf(cerrbuf);solution.close();
			cout << "\nOK: runs without errors\n"
				 << endl;
			comparefile(stoi(s), stoi(s) + 1);
    }

    return 0;
}