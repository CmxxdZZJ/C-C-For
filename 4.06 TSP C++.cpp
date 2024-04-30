#include <vector>		
#include <algorithm>
#include <execution>
#include <random>
#include <thread>
#include <iostream>
using namespace std;
 
const int city_num = 48;
const int population_size = 1000;
const int iteration_times = 2000;
const float crossover_chance = 0.5;
//const float mutation_chance = 0.02;
const int thread_num = 16;
const int best_solution = 10628;
 
class Point
{
public:
	short int id;
	short int x;
	short int y;
public:
	Point(short int _id = 0, short int _x = 0, short int _y = 0) :id(_id), x(_x), y(_y)
	{
	}
};
 
typedef vector<Point> Points;
typedef vector<Point> Individual;
typedef vector<Individual> Population;
 
int GetPointsFromFile(const char filename[], Points& points);
void GetDistanceMatrix(const Points& points);
int GetDistance(const Point& p1, const Point& p2);
int GetPathLength(int index[]);
void InitPopulation(Population& population, const Points& points);
void GreedyInitPath(const Points& points, Individual& indiv);//可有可无，差别不大
void Selection(const Population& population, Population& parents);
void CPX(const Individual& p1, const Individual& p2, Individual& children);//我是摆设
void Mutation(Individual& indiv);
bool Cmp(const Individual& indiv1, const Individual& indiv2);
int Tsp(const Points& points, int shorest_path_length_of_thread[], int tid);
 
int dis[city_num][city_num];
 
//略
 
int main()
{
	clock_t t = clock();
 
	const char filename[100] = "E:\\att48.txt";
	Points points;
 
	GetPointsFromFile(filename, points);
	GetDistanceMatrix(points);
 
	int shorest_path_length = INT_MAX;
	int shorest_path_length_sum = 0;
	int shorest_path_length_of_thread[thread_num];
	int shortest_path_thread_id;
 
	thread td[thread_num];
	for (int i = 0; i < thread_num; i++)
	{
		td[i] = thread(&Tsp, points, shorest_path_length_of_thread, i);
	}
 
	for (int i = 0; i < thread_num; i++)
	{
		td[i].join();
	}
 
	for (int i = 0; i < thread_num; i++)
	{
		shorest_path_length_sum += shorest_path_length_of_thread[i];
		if (shorest_path_length_of_thread[i] < shorest_path_length)
		{
			shorest_path_length = shorest_path_length_of_thread[i];
			shortest_path_thread_id = i;
		}
	}
 
	cout << "最短路径平均值: " << shorest_path_length_sum / thread_num;
	cout << ", 和最优解之间的误差: " << fabs((float)(best_solution - shorest_path_length_sum / thread_num)) / (float)best_solution * 100.0 << "%" << endl;
	cout << "第" << shortest_path_thread_id << "个线程得到最短路径的解最优, ";
	cout << "路径长度: " << shorest_path_length;
	cout << ", 和最优解之间的误差: " << fabs((float)(best_solution - shorest_path_length)) / (float)best_solution * 100.0 << "%" << endl;
	cout << "用时" << ((double)clock() - (double)t) / CLOCKS_PER_SEC << "秒";
 
	return 0;
}