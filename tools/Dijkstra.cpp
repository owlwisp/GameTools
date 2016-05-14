//
//  CDijkstra.cpp
//
//  Created by owlwisp on 14-5-31.
//
//

#include <iostream>
#include <vector>
using namespace std;


#define kDijkstraMax 65535

class Dijkstra
{
public:
    Dijkstra(int **map,int top);
    ~Dijkstra();
    void outputMinPath(vector<int> prevIndex,int startIndex,int endIndex);
    void findPath(int startIndex);
    void findPath(int startIndex,int endIndex);
    void printPath();
private:
//    路径
    vector<vector<int> > _path;
//    路径消耗
    vector<int> _cost;
    
    vector< vector<int> > _map;
};


Dijkstra::Dijkstra(int **map,int top)
{
    vector<int>  temp;
    for (int i = 0;i < top; i++)//将L1初始化到m_map
    {
        for (int j = 0; j < top; j++)
        {
            temp.push_back(*((int*)map + top * i + j));
        }
        _map.push_back(temp);
        temp.clear();
    }
}

void Dijkstra::outputMinPath(vector<int> prevIndex,int startIndex,int endIndex)
{
    vector<int> path;
    int index = endIndex;
    path.push_back(index);
    while(prevIndex[index] != startIndex)
    {
        index = prevIndex[index];
        path.push_back(index);
    }
    path.push_back(startIndex);
    
    vector<int> LPath;
    for (int i = path.size() -1 ; i >=0 ; --i) {
        LPath.push_back(path.at(i));
    }
    _path.push_back(LPath);
}

void Dijkstra::printPath()
{
    for (int i = 0; i < _path.size(); ++i) {
        vector<int> path = _path.at(i);
        
        printf("v%d到v%d的最短路径为:\nv%d",path.at(0),path.at(path.size()-1),path.at(0));
        
        for (int j = 1 ; j< path.size(); ++j) {
            printf("->v%d",path.at(j));
        }
        printf("\n");
        printf("最短路径消耗为%d",_cost[path.at(path.size()-1)]- _cost[path.at(0)]);
        printf("\n\n");
    }
}

void Dijkstra::findPath(int startIndex,int endIndex)
{
    _cost = _map[0];
    vector<bool> isLabel(_cost.size(),false);
    vector<int> dis;
    int count = 0;
    int index = startIndex;
    dis.push_back(index);
    isLabel[index] = true;
    vector<int> prevIndex(_cost.size(),startIndex);
    
    while (count < _cost.size())
    {
        int min = kDijkstraMax;
        
        for(int i = 0;i < _cost.size(); i++)
        {
            if (!isLabel[i] && _cost[i] != -1 && i != index)
            {
                if (_cost[i] < min)
                {
                    min = _cost[i];
                    index = i;
                }
            }
            
        }
        if (index == endIndex)
        {
            break;
        }
        isLabel[index] = true;
        count ++ ;
        dis.push_back(index);
        for (int j = 0; j < _cost.size(); j++)
        {
            if (_cost[j] == -1 && _map[index][j] != -1 && !isLabel[j])
            {
                _cost[j] = _cost[index] + _map[index][j];
                prevIndex[j] = index;
            }
            else if (_map[index][j] != -1 && _cost[index] + _map[index][j] < _cost[j])
            {
                _cost[j] = _cost[index] + _map[index][j];
                prevIndex[j] = index;
            }
        }
    }

    outputMinPath(prevIndex,startIndex,endIndex);
}

void Dijkstra::findPath(int startIndex)
{
	_cost = _map[0]; // v0到各点的最短距离的初始值
	vector<bool> isLabel(_cost.size(),false); // 是否标号 
	vector<int> dis; // 所有标号的点的下标集合，以标号的先后顺序进行存储，实际上是一个以数组表示的栈  
	int count=0;    //被标号顶点的顺序或数目  
	int index = startIndex; // 从初始点开始
    
	dis.push_back(index); // 把已经标号的下标存入下标集中 
	isLabel[index]=true;
	vector<int> prevIndex(_cost.size(),startIndex); //初始化为起始接点 
    
    
	while (true)
	{
        // 第一步：标号v_start,即w[start][0]行中找到距离v_start最近的点
		int min = kDijkstraMax;
        //找出与v_start距离最短的cost的下标
		for(int i = 0; i < _cost.size(); i++)
		{
			if (!isLabel[i] && _cost[i] != -1 && i != index)
			{
                // 如果到这个点有边,并且没有被标号  
				if (_cost[i] < min)
				{
					min = _cost[i];
					index = i; // 记录下这个下标 
				}
			}
			
		}
        
		isLabel[index]=true; //对点进行标号
		count++; //增加一个标定的点 
        
		if (count >= _cost.size())
		{
            //所有点已经遍历完  
			break;
		}
        // 把已经标号的下标存入下标集中
		dis.push_back(index);
        
        // 第二步：加入新的节点后，更新所有不在dis中的节点的m_cost 
		for (int j = 0;j < _cost.size(); j++)
		{
            //这个节点在原来的寻路中不可到达，刚加入的点和这个节点是互联的，并且不在dis中  （添加新点不更新路径）
			if (_map[index][j] != -1  && _cost[j] == -1 && !isLabel[j])
			{
				_cost[j] = _cost[index] + _map[index][j];
				prevIndex[j] = index;//更新最短路径中当前接点的前一个接点 
			}
			else if (_map[index][j] != -1 && _cost[index] + _map[index][j] < _cost[j])
			{
                // 如果以前可达，但现在的路径比以前更短，则更换成更短的路径
                //即先从 
				_cost[j] = _cost[index] + _map[index][j];
				prevIndex[j] = index; //更新最短路径中当前接点的前一个接点  
			}
		}//每个节点到起始节点的最小距离更新完毕 
	}
    
    //如果全部点都遍历完，则Distance中存储的是开始点到各个点的最短路径   
	for (int i = 0; i < _cost.size(); i++)
	{
		outputMinPath(prevIndex, startIndex, i);//输出最短路径
	}
}

void test1(){
    //测试数据1 无向图 实对称矩阵
    int const N1=6;
    int L1[N1][N1] = { \
        { 0,2,-1,1,-1,-1},
        { 2,0,3,2,-1,-1},
        { -1,3,0,3,1,5 },
        { 1,2,3,0,1,-1},
        {-1,-1,1,1,0,2},
        {-1,-1,5,-1,2,0}};
    
    Dijkstra *di = new Dijkstra((int **)L1,N1);
    di->findPath(0,5);
    di->findPath(0);
    

    di->printPath();
}

void test2(){
   //测试数据2 有向图 非对称矩阵
   int const N2=5;
   int L2[N2][N2] =  { \
       { 0,10,-1,30,100 },
       { -1,0,50,-1,-1 },
       { -1,-1,0,-1,10},
       {-1,-1,20,0,60,},
       {-1,-1,-1,-1,0}};

    Dijkstra *di2 = new Dijkstra((int **)L2,N2);
    di2->findPath(0,4);
    di2->findPath(0);
    di2->printPath();
}

int main(){

    test1();
    test2();
    return 1;
}

