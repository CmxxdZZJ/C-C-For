//https://blog.csdn.net/qq_45907357/article/details/125113036
#include<iostream>
#include<vector>
#include<iomanip>
#include<unordered_map>
#include<algorithm>
#include<math.h>
#include<time.h>
 
 
#define GROUP_NUM 100    //种群规模
#define CITY_NUM 15     //城市数量
#define ITERATION_NUM 1000   //最大迭代次数
#define Pc 0.9      //交叉率
#define Pm 0.1     //变异率
using namespace std;
 
//路线类
class Route {
public:
	vector<int> seq;    //路线的城市顺序
	double fitness;   //适应度（定义为城市序列中相邻两城的距离之和的倒数）
	double Ps;  //生存概率（被选择概率）
	double dis; //路线距离
 
	//构造函数
	Route() {
		seq = vector<int>(CITY_NUM + 1);
		fitness = 0;
		Ps = 0;
	}
};
 
//城市坐标类
class City {
public:
	int x;  //横坐标
	int y;  //纵坐标
};
 
//为自定义类(Route)制定排序规则
//升序排列，即生存概率高的排在后面
bool my_cmp(Route r1, Route r2) {
	return r1.Ps < r2.Ps;
}
 
//城市之间的距离矩阵
vector<vector<double>> dis(CITY_NUM, vector<double>(CITY_NUM, 0.0));
 
//种群
vector<Route> group(GROUP_NUM);
 
//城市
vector<City> city(CITY_NUM);
 
//城市初始化函数，随机生成CITY_NUM个二维坐标节点，计算城市间的距离并存在距离矩阵中
void city_init() {
	//设城市全部坐落在100 * 100的二维平面内
	//种下随机种子，使每次运行生成的城市坐标不同
	srand((unsigned)time(NULL));
	cout << "生成的随机城市坐标：" << endl;
	for (int i = 0; i < CITY_NUM; i++) {
		//为每个城市随机生成坐标
		city[i].x = rand() % 100;
		city[i].y = rand() % 100;
		cout << i << " " << '(' << city[i].x << ", " << city[i].y << ')' << endl;
	}
 
	//计算城市距离,城市i到城市j的距离与城市j到i的距离相等
	for (int i = 0; i < CITY_NUM; i++) {
		for (int j = i; j < CITY_NUM; j++) {
			int temp1 = (city[i].x - city[j].x) * (city[i].x - city[j].x);
			int temp2 = (city[i].y - city[j].y) * (city[i].y - city[j].y);
			dis[i][j] = sqrt(temp1 + temp2);
			dis[j][i] = dis[i][j];
		}
	}
}
 
//种群初始化函数,生成GROUP_NUM个初始随机访问城市序列
void group_init() {
	srand((unsigned)time(NULL));//随机数发生器
	for (int i = 0; i < GROUP_NUM; i++) {//一共生成GROUP_NUM个随机路线
		//用哈希表防止序列中生成重复的城市
		unordered_map<int, int> mp;
		for (int j = 0; j < CITY_NUM; j++) {
			int num = rand() % CITY_NUM;
			//如果随机生成的数重复了，则重新生成直到不重复为止
			while (mp[num] != 0) {//如果已经生成过了则重新生成
				num = rand() % CITY_NUM;
			}
			mp[num]++;
			group[i].seq[j] = num;//路线添加随机点
		}
		group[i].seq[CITY_NUM] = group[i].seq[0];//最后一个航点为起点
	}
	/*
	cout << "初始种群：" << endl;
	for(int i = 0; i < GROUP_NUM; i++) {
		for(int j = 0; j < CITY_NUM; j++) {
			cout << group[i].seq[j] << " ";
		}
		cout << endl;
	}*/
 
}
 
 
 
//计算初始种群中每个个体的适应度及生存概率
//适应度设置为序列中相邻两城之间的距离之和
void cal_group() {
	//种群总适应度
	double total_fit = 0.0;
 
	//计算每个个体的适应度
	for (int i = 0; i < GROUP_NUM; i++) {
		double total_dis = 0;
		for (int j = 1; j <= CITY_NUM; j++) {
			total_dis += dis[group[i].seq[j]][group[i].seq[j - 1]];
		}
		group[i].dis = total_dis;
		//个体的适应度为总距离
		group[i].fitness = 1.0 / total_dis;
		//测试计算出来的路径和是否正确
		//cout << total_dis << " " << group[i].fitness << endl;
 
		total_fit += group[i].fitness;
	}
 
	//计算每个个体的生存概率（被选择概率）,为个体适应度 / 总适应度
	for (int i = 0; i < GROUP_NUM; i++) {
		group[i].Ps = group[i].fitness / total_fit;
	}
}
 
//打印种群信息
void show() {
	for (int i = 0; i < GROUP_NUM; i++) {
		for (int j = 0; j <= CITY_NUM; j++) {
			if (j == CITY_NUM) {
				cout << group[i].seq[j];
			}
			else {
				cout << group[i].seq[j] << "->";
			}
		}
		cout << setprecision(4) << "   适应度为：" << group[i].fitness << "  生存概率为：" << group[i].Ps << endl;
	}
}
 
//选择
void select() {
 
	//计算累计概率
	vector<double> acc_p(GROUP_NUM);//累计概率，例如原概率0.1 0.3 0.3 0.3，累计概率为0.1 0.4 0.7 1.0
	acc_p[0] = group[0].Ps;         //其含义为，越优的路径，越被排在vector后面，这个路线被选择到的概率越大
	for (int i = 1; i < GROUP_NUM; i++) {
		acc_p[i] = acc_p[i - 1] + group[i].Ps;
	}
 
	//记录被选择的个体，利用赌轮选择法，随机生成0~1之间一个数，根据计算出来的累计概率选择个体
	vector<Route> sel_individual(GROUP_NUM);
	srand((unsigned)time(NULL));
	for (int i = 0; i < GROUP_NUM; i++) {
		//生成0~1的随机数,4位小数
		float random = rand() % (10000) / (float)(10000);
		//cout << random << " ";
 
		for (int j = 0; j < acc_p.size(); j++) { //有可能好几条相同的路径被选中
			if (random <= acc_p[j]) {
				//cout << random << " " << acc_p[j] << endl;
				sel_individual[i] = group[j];//被选择的路径越好，被选中的概率越大，好路径被选择，差路径被淘汰
				break;
			}
		}
	}
 
	//被选择的种群覆盖初始种群
	for (int i = 0; i < GROUP_NUM; i++) {
		group[i] = sel_individual[i];
	}
 
	/*cout << "打印经过自然选择后的种群序列：" << endl;
	for(int i = 0; i < GROUP_NUM; i++) {
		cout << i << "、" << " ";
		for(int j = 0; j < CITY_NUM; j++) {
			cout << group[i].seq[j] << " ";
		}
		cout << "适应度为：" << group[i].fitness << "  生存概率为："  << group[i].Ps << endl;
	}*/
}
 
//交叉（交配）算法
//第k（k=0、2、4、...、2n）个个体和k+1个个体有一定的概率交叉变换
//设置一个0~1之间的随机数，若在Pc（交配率）范围内，则该该个体k与下一个个体k+1进行交配
void mating() {
	//随机生成子代交配时DNA交换的数量(1~CITY_NUM / 2)
	srand((unsigned)time(NULL));
	int change_num = (rand() % CITY_NUM / 2) + 1;  //0~14之间的交换数字
	//cout << "交换DNA数量：" << change_num << endl;
 
	//开始交配
	for (int i = 0; i < CITY_NUM; i += 2) {
		//生成0-1之间的随机数(3位小数)
		float random = rand() % (1000) / (float)(1000);
		//在交配率以内，则该个体i与下一个个体i+1进行交配
		if (random < Pc) {//0.9的交叉概率
			//随机生成交配点
			int point = rand() % (CITY_NUM - change_num);
 
			//cout << i << " 与 " << i + 1 << " 进行交配，断点：" << point << endl;
 
			//先将双亲的交配片段进行互换，并用哈希映射记录，然后解决基因冲突
			unordered_map<int, int> hash1;
			for (int j = point; j < change_num + point; j++) {
				int a = group[i].seq[j];//i的点
				int b = group[i + 1].seq[j];//i+1的点
				if (hash1.find(a) != hash1.end()) {
					a = hash1[a];//为了解决下面的重复哈希映射，只保留一个
				}
				if (hash1.find(b) != hash1.end()) {
					b = hash1[b];
				}
				hash1[a] = b;//a对应b
				hash1[b] = a;//b对应a
				swap(group[i].seq[j], group[i + 1].seq[j]);//交换第i和i+1个路径中a~b的点
			}
			//处理双亲交配后可能产生的基因冲突问题(断点前)
			for (int j = 0; j < point; j++) {
				if (hash1.find(group[i].seq[j]) != hash1.end()) {
					group[i].seq[j] = hash1[group[i].seq[j]];
				}
				if (hash1.find(group[i + 1].seq[j]) != hash1.end()) {
					group[i + 1].seq[j] = hash1[group[i + 1].seq[j]];
				}
			}
			//断点后
			for (int j = point + change_num; j < CITY_NUM; j++) {
				if (hash1.find(group[i].seq[j]) != hash1.end()) {
					group[i].seq[j] = hash1[group[i].seq[j]];
				}
				if (hash1.find(group[i + 1].seq[j]) != hash1.end()) {
					group[i + 1].seq[j] = hash1[group[i + 1].seq[j]];
				}
			}
		}
		//最后一个城市的下一个城市是第一个城市
		group[i].seq[CITY_NUM] = group[i].seq[0];
	}
 
 
	/*
	//打印交配过后的种群
	for(int i = 0; i < GROUP_NUM; i++) {
		cout << i << "、" << " ";
		for(int j = 0; j < CITY_NUM; j++) {
			cout << group[i].seq[j] << " ";
		}
		//cout << "适应度为：" << group[i].fitness << "  生存概率为："  << group[i].Ps << endl;
		cout << endl;
	}*/
}
 
//变异算法
//每个算子有一定概率（变异概率）基因多次对换。
//对每个个体，若满足变异概率，则随机生成两个不相等的范围在[0,城市数 - 1]之间的随机整数。将该个体在这两个随机整数对应的位置的城市编号对换
//进行上述n次对换，n是一个[1,城市数]之间的随机整数
void mutate() {
	srand((unsigned)time(NULL));
	for (int i = 0; i < GROUP_NUM; i++) {
		//生成0-1之间的随机数(4位小数)
		float random = rand() % (10000) / (float)(10000);
		//cout << random << " ";
		if (random < Pm) {//0.1
			//cout << i << " 号个体产生变异" << endl;
			//随机生成基因对换次数
			int exchange_times = rand() % CITY_NUM + 1;
			while (exchange_times > 0) {
				//随机生成两个不相等的范围在[0,城市数 - 1]之间的随机数
				int a = rand() % CITY_NUM;
				int b = rand() % CITY_NUM;
				swap(group[i].seq[a], group[i].seq[b]);//随机变异
				exchange_times--;
			}
		}
		//最后一个城市的下一个城市是第一个城市
		group[i].seq[CITY_NUM] = group[i].seq[0];
	}
	/*cout << endl << "打印变异过后的种群" << endl;
	for(int i = 0; i < GROUP_NUM; i++) {
		cout << i << "、" << " ";
		for(int j = 0; j < CITY_NUM; j++) {
			cout << group[i].seq[j] << " ";
		}
		//cout << "适应度为：" << group[i].fitness << "  生存概率为："  << group[i].Ps << endl;
		cout << endl;
	}*/
}
 
 
 
int main()
{
	int it = 0;   //迭代次数
	//随机生成初始城市坐标
	city_init();
	//随机生成初始种群（100条随机路线）
	group_init();
	//计算每个路径的代价及被选择的概率
	cal_group();
	//对路径进行排序，代价小的排在后面
	sort(group.begin(), group.end(), my_cmp);
 
	//show();//打印100个种群信息
	cout << endl;
 
	cout << "初代“最优”路线为：";
	for (int i = 0; i < CITY_NUM + 1; i++) {
		cout << group[GROUP_NUM - 1].seq[i] << " ";
	} cout << "适应度为：" << group[GROUP_NUM - 1].fitness << endl;
 
	cout << "该路线长度为：" << group[GROUP_NUM - 1].dis << endl;
 
	cout << "该路线对应的坐标点分别为：" << endl;
	for (int i = 0; i < CITY_NUM + 1; i++) {
		int t = group[GROUP_NUM - 1].seq[i];
		if (i == CITY_NUM) {
			cout << '(' << city[t].x << ", " << city[t].y << ')' << endl;
		}
		else {
			cout << '(' << city[t].x << ", " << city[t].y << ')' << "->";
		}
	}
 
	while (it <= ITERATION_NUM) {//迭代1000次
		//计算适应度以及生存概率
		cal_group();
 
		//在种群中选择个体
		select();
 
		//种群进行交配
		mating();
 
		//种群中的个体产生变异
		mutate();
 
		it++;
 
	} cout << endl;
 
	//代价最小的排在最后面
	sort(group.begin(), group.end(), my_cmp);
 
	//show();//打印种群信息
	//cal_group();
 
	cout << "经过" << ITERATION_NUM << "次迭代后：" << endl;
	cout << "“最优”路线为：";
	for (int i = 0; i < CITY_NUM + 1; i++) {
		cout << group[GROUP_NUM - 1].seq[i] << " ";
	} cout << "适应度为：" << group[GROUP_NUM - 1].fitness << endl;
 
 
	cout << "该路线长度为：" << group[GROUP_NUM - 1].dis << endl;
 
	cout << "该路线对应的坐标点分别为：" << endl;
	for (int i = 0; i < CITY_NUM + 1; i++) {
		int t = group[GROUP_NUM - 1].seq[i];
		//cout << '(' << city[t].x << ", " << city[t].y << ')' << endl;
		if (i == CITY_NUM) {
			cout << '(' << city[t].x << ", " << city[t].y << ')' << endl;
		}
		else {
			cout << '(' << city[t].x << ", " << city[t].y << ')' << "->";
		}
	}
	return 0;
}
 