// LevyWalk.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int num_of_nodes = 100;
int max_x = 100, max_y = 100;
int dimension[2] = { 100,100 }; // (max_x , max_y)

double FL_EXP = 2.6, FL_MAX = 50.0, WT_EXP = -1.8, WT_MAX = 100.0; // LevyWalk parameter
int main(){
	srand(time(NULL));
	LevyWalk();
	system("pause");
	return 0;
}

double Uniform_distri(double min, double max) {
	double x = (double)rand() / (RAND_MAX + 1.0);
	double res = x * (max - min) + min;
	return res;
}

double PowerLaw_distri(double ALPHA, double min, double max) {
	double x = (double)rand() / (RAND_MAX + 1.0);
	double res = pow(((pow(max, (ALPHA + 1.0)) - 1.0) * x + 1.0),1.0 / (ALPHA + 1.0));
	return res;
}

double FL_distri() {
	return PowerLaw_distri(FL_EXP, 1.0, FL_MAX);
}

double Velocity_distri(double value) {
	return sqrt(value) / 10;
}

void LevyWalk() {
	int policy = 0; //0 : reflect 1 : Wrap
	StochasticWalk(policy);
}

void StochasticWalk(int policy) {
	int ndim = sizeof(dimension) / sizeof(*dimension);
	vector<int> NODES = arange(num_of_nodes);
	//assign node's positions, flight lengths, velocities
	vector<double> x_pos(num_of_nodes, 0);
	vector<double> y_pos(num_of_nodes, 0);
	vector<double> fl(num_of_nodes, 0);
	vector<double> velocity(num_of_nodes, 0); 
	for (int i = 0; i < num_of_nodes; i++) {
		x_pos[i] = Uniform_distri(0.0, dimension[0]); // (0. max_x)
		y_pos[i] = Uniform_distri(0.0, dimension[1]); // (0, max_y)
		fl[i] = FL_distri();
		velocity[i] = Velocity_distri(fl[i]);
	}
	//assign nodes' movements
	vector<double> direction_x(num_of_nodes, 0);
	vector<double> direction_y(num_of_nodes, 0);
	vector<double> movement_x(num_of_nodes, 0);
	vector<double> movement_y(num_of_nodes, 0);
	for (int i = 0; i < num_of_nodes; i++) {
		direction_x[i] = Uniform_distri(0.0, 1.0) - 0.5;
		direction_y[i] = Uniform_distri(0.0, 1.0) - 0.5;
		//calculating norm
		double norm = sqrt(pow(direction_x[i], 2) + pow(direction_y[i], 2));
		movement_x[i] = direction_x[i] / norm * velocity[i];
		movement_y[i] = direction_y[i] / norm * velocity[i];
	}
	//Starting with no waiting time
	vector<double> waiting(num_of_nodes, 0);
	int t = 0;
	while (t < 100) { //true
		vector<double> arrived;
		for (int i = 0; i < num_of_nodes; i++) {
			x_pos[i] += movement_x[i];
			y_pos[i] += movement_y[i];
			fl[i] -= velocity[i];
			if (velocity[i] > 0 && fl[i] <= 0.0) arrived.push_back(i);
		}
		//step back (超過飛行距離)
		if (arrived.size() > 0) {
			for (int i = 0; i < arrived.size(); i++) {
				double diff = fl[arrived[i]] / velocity[i];
				x_pos[arrived[i]] += diff * movement_x[arrived[i]];
				y_pos[arrived[i]] += diff * movement_y[arrived[i]];
			}
		}
		//apply border
		check(policy, x_pos, y_pos, movement_x, movement_y);
		//updating the moving nodes,if nodes complete fl then apply new fl, v, direc, movement
		if (arrived.size() > 0) {
			for (int i = 0; i < arrived.size(); i++) {
				fl[arrived[i]] = FL_distri();
				velocity[arrived[i]] = Velocity_distri(fl[arrived[i]]);
				double v = velocity[arrived[i]];
				direction_x[arrived[i]] = Uniform_distri(0.0, 1.0) - 0.5;
				direction_y[arrived[i]] = Uniform_distri(0.0, 1.0) - 0.5;
				//calculating norm
				double norm = sqrt(pow(direction_x[arrived[i]], 2) + pow(direction_y[arrived[i]], 2));
				movement_x[arrived[i]] = direction_x[arrived[i]] / norm * velocity[arrived[i]];
				movement_y[arrived[i]] = direction_y[arrived[i]] / norm * velocity[arrived[i]];
			}
		}
		//output the position(pos_x,pos_y) at t;
		cout << "Time: " << t << endl;
		for (int i = 0; i < num_of_nodes; i++) {
			cout << i << " : " << x_pos[i] << " " << y_pos[i] << endl;
		}
		t++;
	}
}

void check(int border_policy, vector<double>& x_pos, vector<double>& y_pos, vector<double>& movement_x, vector<double>& movement_y) {
	if (border_policy) reflect(x_pos, y_pos, movement_x, movement_y);
	else               wrap(x_pos, y_pos, movement_x, movement_y);
}
void reflect(vector<double>& x_pos, vector<double>& y_pos, vector<double>& movement_x, vector<double>& movement_y) {
	for (int i = 0; i < num_of_nodes; i++) {
		if (x_pos[i] < 0) {
			x_pos[i] = -x_pos[i];
			movement_x[i] = -movement_x[i];
		}
		else if (x_pos[i] > max_x) {
			x_pos[i] = 2 * max_x - x_pos[i];
			movement_x[i] = -movement_x[i];
		}
		if (y_pos[i] < 0) {
			y_pos[i] = -y_pos[i];
			movement_y[i] = -movement_y[i];
		}
		else if (y_pos[i] > max_y) {
			y_pos[i] = 2 * max_y - y_pos[i];
			movement_y[i] = -movement_y[i];
		}
	}
}

void wrap(vector<double>& x_pos, vector<double>& y_pos, vector<double>& movement_x, vector<double>& movement_y) {
	for (int i = 0; i < num_of_nodes; i++) {
		if (x_pos[i] < 0) x_pos[i] += max_x;
		else if (x_pos[i] > max_x) x_pos[i] -= max_x;
		if (y_pos[i] < 0) y_pos[i] += max_y;
		else if (y_pos[i] > max_y) y_pos[i] -= max_y;
	}
}

vector<int> arange(int n) {
	vector<int> res;
	for (int i = 0; i < n; i++) {
		res.push_back(i);
	}
	return res;
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的秘訣: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
