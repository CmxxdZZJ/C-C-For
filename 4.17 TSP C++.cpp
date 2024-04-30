#include<iostream>
#include<cmath>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>
#define ERROR -1
using namespace std;

typedef struct City
{
	int x,y;	// 城市坐标
}City;
City cities[10];
double distance(int a, int b)	// 计算两城市间距离
{
	return abs((cities[a].x - cities[b].x) ^ 2 + (cities[a].y - cities[b].y) ^ 2);
}

int gene_sum = 9;	// 染色体上的基因数
class Population;	// 向前引用，种群类
class Chro	// Chromoesome染色体
{
public:
	Chro();	// 构造函数，将适应度初始化为0
	void RandomInit();	// 随机初始化
	double fx();		// 适应度函数
	void display();		// 展示基因序列
	void copy(Chro s);	// 复制
	void exchange(Chro &s);// 互换
	void variation();	// 变异
	friend int find(int gene[], int start, int end, int x);	// 寻找基因x在染色体上的位置
	friend class Population;
private:
	int *gene;
	double fits;	// 适应度
};
Chro::Chro()
{
	gene = new int[gene_sum];
	fits = 0;
}
void Chro::RandomInit()	// 随机初始化染色体上的基因序列
{
	int i;
	time_t t;
	for (i = 0; i < gene_sum; i++) {
		gene[i] = i;
	}
	srand((int)time(&t));// 确保不会生成重复的随机数
	for (i = 0; i < 1+int(gene_sum/2); i++) {
		swap(gene[i], gene[i + rand() % (gene_sum - i)]);
	}
	fx();
	Sleep(1000);
}
double Chro::fx()	//	适应度函数fx
{
	double f = 0;
	for (int i = 0; i < gene_sum; i++){
		f += distance(gene[i], gene[(i + 1) % gene_sum]);
	}
	fits = 1.0 / f;// 总距离越大，适应度越小
	return fits;
} 
void Chro::display()	// 展示基因序列和该染色体适应度
{
	cout << "    [";
	for (int i = 0; i < gene_sum; i++) {
		cout << " " << gene[i];
	}
	cout << " ],  fits= " << fx()<<", distance= "<<1/fx();
}
void Chro::copy(Chro s)	//复制
{
	for (int i = 0; i < gene_sum; i++){
		gene[i] = s.gene[i];
	}
	fits = fx();	// 重新计算适应度
}
int find(int gene[], int start, int end, int x)// 在gene序列中查找x基因的位置并返回
{
	for (int i = start; i <= end; i++) {
		if (gene[i] == x) {
			return i;
		}
	}
	return ERROR;
}
void Chro::exchange(Chro &s)	//将当前染色体与另一染色体s进行基因片段交换
{
	int i, j = 0, k = 0, repeat;
	int pos = rand() % (gene_sum - 4);	// 随机选择交叉位置（由于要交换3或4个基因，所以交叉位置只能在[1，n-4]内）
	int num = 3 + rand() % 2;	// 随机选择交叉的基因数，最小为3，最大为4
	/*
	cout << endl; 查看发生交换的位置和位数
	cout << "pos: " << pos << ", num: " << num << endl;*/

	int *segment1 = new int[gene_sum];	// 用于记录交换后当前染色体上的基因
	int *segment2 = new int[gene_sum];	// 用于记录交换后另一染色体上的基因
	for (i = 0; i < gene_sum; i++) {
		if (i >= pos && i < pos + num) {
			segment1[i] = s.gene[i];
			segment2[i] = gene[i];
		}
		else {
			segment1[i] = gene[i];
			segment2[i] = s.gene[i];
		}
	}

	int *mapping1 = new int[4];	// 当前染色体中间段的映射
	int *mapping2 = new int[4];	// 另一染色体中间段的映射
	for (i = 0; i < 4; i++) {
		mapping1[i] = ERROR;	// 初值全部为-1
		mapping2[i] = ERROR;
	}
	for (i = pos; i < pos + num; i++) {
		repeat = find(segment1, pos, pos + num - 1, gene[i]);
		if (repeat == ERROR) {
			mapping1[j++] = gene[i];	
		}
		repeat = find(segment2, pos, pos + num - 1, s.gene[i]);
		if (repeat == ERROR) {
			mapping2[k++] = s.gene[i];
		}
	}
	/* 查看映射
	cout << "map1   " << "map2" << endl;
	for (k = 0; k < 4; k++) {
		cout << mapping1[k] << "     " << mapping2[k] << endl;
	}*/

	j = k = 0;
	for (i = pos; i < pos + num; i++) {// 将重复的基因替换为映射中的基因
		repeat = find(gene, 0, pos - 1, segment1[i]);
		if (repeat != ERROR) {
			segment1[repeat] = mapping1[j++];
		}
		repeat = find(gene, pos + num, gene_sum - 1, segment1[i]);
		if (repeat != ERROR) {
			segment1[repeat] = mapping1[j++];
		}
		repeat = find(s.gene, 0, pos - 1, segment2[i]);
		if (repeat != ERROR) {
			segment2[repeat] = mapping2[k++];
		}
		repeat = find(s.gene, pos + num, gene_sum - 1, segment2[i]);
		if (repeat != ERROR) {
			segment2[repeat] = mapping2[k++];
		}
	}
	for (i = 0; i < gene_sum; i++) {
		gene[i] = segment1[i];	// 交叉后的该染色体
		s.gene[i] = segment2[i];// 交叉后的另一染色体
	}
	delete segment1;
	delete segment2;
	delete mapping1;
	delete mapping2;
}
void Chro::variation()
{
	int pos = rand() % 8;	// 随机选择变异位置
	int temp = gene[pos];	// 将被选中的基因和后面一位基因交换
	gene[pos] = gene[pos + 1];
	gene[pos + 1] = temp;
}

Chro solution;	// 用于记录有史以来最好的染色体
int best_generation = 0;	// 用于记录最好的染色体所在的代数
int chro_sum = 20;	// 种群中染色体数
class Population
{
public:
	Population();	// 构造函数
	void best_fit();// 查找适应度最高的染色体
	void select();	// 选择
	void cross();	// 交叉
	void variation();// 种群变异
	void display();	// 显示种群内染色体
	int generation; // 种群代数
private:
	Chro *chromoesomes;	// 染色体
	double Σf;	// 种群适应度之和
	double *P;	// 选择概率
};
Population::Population()
{
	int i;
	generation = 1;
	chromoesomes = new Chro[chro_sum];
	for ( i = 0; i < chro_sum; i++){
		chromoesomes[i].RandomInit();
	}
	Σf = 0;
	P = new double[chro_sum];
	for (i = 0; i < chro_sum; i++) {
		Σf += chromoesomes[i].fits;
	}
	for (i = 0; i < chro_sum; i++) {
		P[i] = chromoesomes[i].fits / Σf;
	}
}
void Population::best_fit()	// 查找适应度最大的染色体并返回其编号
{
	int best = 0;
	for (int i = 1; i < chro_sum; i++){
		if (chromoesomes[best].fx() < chromoesomes[i].fx()) {
			best = i;
		}
	}
	if (chromoesomes[best].fx() > solution.fits) {
		solution.copy(chromoesomes[best]);
		best_generation = generation;
	}
	cout << "  The best fit in generation" << generation << " is: " << endl;
	cout << "  chromoesomes" << best + 1 << ": ";
	chromoesomes[best].display();
	cout << endl;
}
void Population::select()	// 种群选择
{
	int i, j;
	int *selected = new int[chro_sum];	// 用于记录被选中的染色体号
	double r;
	double *q = new double[chro_sum];	// 用于记录积累概率
	Chro *cp = new Chro[chro_sum];
	q[0] = P[0];	// 积累概率
	cout << endl;
	cout << "  Accumulation of probabilities" << endl;	// 打印积累概率
	cout << "  q1= " << q[0] << endl;
	for (i = 1; i < chro_sum; i++) {
		q[i] = q[i - 1] + P[i];
		cout << "  q" << i + 1 << "= " << q[i] << endl;
	}
	cout << "\n  Roulette wheel" << endl;// 轮盘赌，产生随机数
	srand(time(NULL));//设置随机数种子，使每次产生的随机序列不同
	for (int i = 0; i < chro_sum; i++){
		r = rand() % (10000) / (double)(10000);
		cout << "  r" << i + 1 << "= " << r << endl;
		if (r <= q[0]) {
			selected[i] = 0;	// 选中第一个染色体
		}
		for (j = 0; j < chro_sum - 1; j++){
			if (q[j] <= r && r <= q[j + 1]) {
				selected[i] = j + 1;	// 选中第j+1个基因
			}
		}
		cp[i].copy(chromoesomes[i]);
	}
	for (i = 0; i < chro_sum; i++){
		chromoesomes[i].copy(cp[selected[i]]);
	}
	delete selected;
	delete q;
	delete cp;
}
void Population::cross()	// 种群交叉
{
	for (int i = 0; i < chro_sum; i += 2) {
		chromoesomes[i].exchange(chromoesomes[i + 1]);
	}
}
void Population::variation()	// 种群变异
{
	int probability = rand() % 100;	// 变异积累为1%
	if (probability==1){
		int x = rand() % chro_sum;	// 随机选择一个染色体变异
		cout << "  The chromoesome " << x << " is variated!" << endl;
		chromoesomes[x].variation();
	}
	generation++;	// 至此，种群进化一代
}
void Population::display()// 依次输出每一条染色体（每种方案）
{
	cout << endl;
	int i;
	for (i = 0; i < chro_sum; i++) {	
		cout << "  chromoesomes" << i + 1 << ": ";
		chromoesomes[i].display();
		cout << endl;
	}
	cout << endl;
}

int main()
{
	cout << "  Input the number of cities:  ";	// 输入城市数(基因数)
	cin >> gene_sum;
	cout << "  Input the number of chromoesomes in population:  ";// 输入染色体数
	cin >> chro_sum;
	cout << "  Input the location of cities:" << endl;// 输入每个城市坐标(x, y)
	for (int i = 0; i < gene_sum; i++){
		cout <<"\t"<< i + 1 << ": ";
		cin >> cities[i].x >> cities[i].y;
	}
	Population P0;
	cout << "\n  Generation=" << P0.generation << endl;
	P0.display();
	P0.best_fit();
	int T;	// 进化的代数
	cout << "  Input the T:  ";
	cin >> T;
	for (int i = 0; i < T; i++) {
		cout << endl << "  After select: ";	// 选择
		P0.select();
		P0.display();
		cout << endl << "  After cross: ";	// 交叉
		P0.cross();
		P0.display();
		cout << endl << "  After variation: ";// 变异
		P0.variation();
		cout << "  Generation=" << P0.generation << endl;
		P0.display();
		P0.best_fit();
		system("pause");
		system("cls");
	}
	cout << "  The best solution in history is:" << endl;	// 打印进化过程中最好的染色体，即解决方案
	cout << "  in generation" << best_generation;	
	solution.display();
	cout << endl;
	system("pause");
	return 0;
}

