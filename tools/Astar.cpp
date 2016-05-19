// A*寻路
#include <stdio.h>
#include <vector>
#include <cstdlib>

struct ASNode
{
    ASNode() : _i(-1)
                , _j(-1)
                , _f(0)
                , _h(0)
                , _g(0)
    {

    }

    ASNode(int i,int j)  : _i(i)
                            , _j(j)
                            , _f(0)
                            , _h(0)
                            , _g(0)
    {

    }

    bool operator==(const ASNode &another)const{
        return _i == another._i && _j == another._j;
    }
    
    //当前位置
    int _i;
    int _j;
    //父结点
    ASNode * _parentNode;
    //从开始结点移动到此结点的移动消费值
    int _g;
    //从此节点移动到结束节点的移动消费值（如曼哈顿方法产生的估值）
    int _h;
    //总移动消费值
    int _f;
};

//怪物寻路逻辑说明
/*
 1. 统一用ThreadCache进行寻路
 2. 如果怪物当前没有路可以走，则进行发呆或者播放动画等行为
 3. 每格一个行动时间片（时间片可以通过时间控制，这样可以控制怪物灵敏度，让怪物的ai体现出来）进行获取多线程路径
 4. 当怪物没有路径的时候，需要向线程提交寻路请求
 5. 当地图信息改变时，通过设置地图信息完成
 6. 地图信息改变时 怪物处理
    1.新增地图信息在自己路径上,所有怪直接发呆，并提交寻路数据
    2.销毁地图信息时
        1. 一种怪,直接发呆,并提交寻路信息
        2. 一种怪，按照原来继续行走
 7. 地图信息改变时，ThreadCache 需要校验自己cache数组中数据的正确性
 */

class AStar {
public:
    //成员函数
    //修改地图中某一块的数据内容
    void setMapSignWithIndex(int x,int y, int sign);
    //获取地图中某个点的信息
     int getMapSignWithIndex(int x,int y);
    //打印地图数据
    void printStar();
    //打印寻找好的路径
    void printPath( std::vector
                   <ASNode> &path );
    //寻找路径
    std::vector<ASNode> findPath( int src_i,int src_j,int dest_i,int dest_j);
    
private:
    //私有成员函数
    bool init();
    void initData();
	void initMap();
    
	std::vector<ASNode> createPath( ASNode *nearNode ,int src_i,int src_j);
    
public:
    virtual ~AStar();
    static AStar* create(int ** map,int width,int height,bool useApproximateDest = false);
    virtual const char* objectType() {return "AStar"; }
    
private:
    AStar();
    AStar(int ** map,int width,int height,bool useApproximateDest);
    
private:
    //成员变量
    int _width;
    int _height;
    int ** _map;
    
    //开启结点列表
	std::vector<ASNode*> _openList;
    //关闭结点列表
    std::vector<ASNode*> _closeList;
    //结点缓冲
	std::vector<std::vector<ASNode> > _cacheArray;
    
    //是否开启近似结点
    bool _isUseApproximateDest;
};






using namespace std;

//水平垂直移动的花费
static const int HORIZONTAL_VERTICAL_COST = 10;
//对角线移动花费
static const int DIAGONAL_COST = 14;

AStar* AStar::create(int ** map,int width,int height,bool useApproximateDest){
    AStar*  instance = new AStar(map,width,height,useApproximateDest);
    if (instance->init()) {
        return instance;
    }
    return nullptr;
}

AStar::AStar()  : _width(0)
                , _height(0)
                , _map(nullptr)
                , _isUseApproximateDest(false)
{

}

AStar::AStar(int ** map,int width,int height,bool useApproximateDest)  : _map(map)
                                                , _width(width)
                                                , _height(height)
                                                , _isUseApproximateDest(useApproximateDest)
{
    
}


AStar::~AStar(){
    _map = nullptr;
}


//私有成员函数
bool AStar::init(){

    initData();
    
    
    return true;
}

void AStar::initData(){
    
}

void AStar::initMap()
{
    _openList.clear();
    _closeList.clear();
    _cacheArray.clear();
    
    _cacheArray = vector< vector< ASNode > >(_height);
    for (unsigned int i = 0; i< _cacheArray.size(); i++)
    {
        _cacheArray[i] = vector< ASNode >(_width);
        for(unsigned int j = 0; j< _cacheArray[i].size(); j++)
        {
            _cacheArray[i][j] = ASNode(i,j);
        }
    }
}


//    修改地图中某一块的数据内容
void AStar::setMapSignWithIndex(int x,int y, int sign){
    *((int*)_map + _width * x + y) = sign;
}
//    获取地图中某个点的信息
int AStar::getMapSignWithIndex(int x,int y){
    return *((int*)_map + _width * x + y);
}


void AStar::printStar()
{
    for (int i = 0;i < _width;i++)
    {
        for (int j = 0;j < _height;j++)
        {
            printf("%d,",*((int*)_map + _width * i + j));

        }
        printf("\n");
    }
}



void AStar::printPath( vector<ASNode> &path )
{
    if (path.empty())
    {
        printf("Error!The path is empty!");
        return;
    }

    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            if (count(path.begin(),path.end(),ASNode(i,j)))
            {
                printf(",");
            }
            else
            {
                if (*((int*)_map + _width * i + j) == -1)
                {
                    printf("*,");
                }
                else if (*((int*)_map + _width * i + j) == 2)
                {
                    printf("2,");
                }
                else if (*((int*)_map + _width * i + j))
                {
                    printf("1,");
                }
                else
                {
                    printf("0,");
                }
            }
        }

        printf("\n");
    }
    printf("\n");
    printf("\n");
    printf("\n");

}



vector<ASNode> AStar::findPath( int src_i,int src_j,int dest_i,int dest_j )
{
    initMap();
    
    // 1. 把起始格添加到开启列表。
    _openList.push_back( &_cacheArray[src_i][src_j]);
    
    // 2. 重复如下的工作：
    
    //查找成功
    bool isSucceed;
    
    while (true)
    {
        // d) 停止，当你
        // *没有找到目标格，开启列表已经空了。这时候，路径不存在。
        if (_openList.empty())
        {
            isSucceed=false;
            break;
        }
        
        // a) 寻找开启列表中F值最低的格子。我们称它为当前格。
        ASNode *currentNode = _openList[0];
        for (unsigned int i = 1; i < _openList.size();i++)
        {
            if( _openList[i]->_f < currentNode->_f)
            {
               currentNode = _openList[i];
            }
        }
        
        // b) 把它切换到关闭列表。
        _openList.erase(remove(_openList.begin(),_openList.end(),currentNode));
        _closeList.push_back(currentNode);
        
        //d) 停止，当你
        //*把目标格添加进了关闭列表(注解)，这时候路径被找到
        if (currentNode== &_cacheArray[dest_i][dest_j])
        {
            isSucceed=true;
            break;
        }
        
        // c) 对相邻的8格中的每一个？
        for (int i = currentNode->_i - 1; i< currentNode->_i + 2; i++)
        {
            for (int j = currentNode->_j - 1; j< currentNode->_j + 2; j++)
            {
                //忽略越界
                if (i < 0||j < 0|| i >= _height||j >= _width)
                {
                    continue;
                }
                
                // * 如果它不可通过，略过它。(约定：非0即为障碍物)
                if (*((int*)_map + _width * i + j))
                {
                    continue;
                }
                
                //* 如果他已经在关闭列表中，略过它。
                if (count(_closeList.begin(),_closeList.end(),&_cacheArray[i][j]))
                {
                    continue;
                }
                
                if (count(_openList.begin(),_openList.end(),&_cacheArray[i][j]))
                {
                    // * 如果它已经在开启列表中
                    int varGToParent;
                    if (i == currentNode->_i || j == currentNode->_j)
                        varGToParent=HORIZONTAL_VERTICAL_COST;
                    else
                        varGToParent=DIAGONAL_COST;
                    
                    // *用G值为参考检查新的路径是否更好。更低的G值意味着更好的路径。
                    if (currentNode->_g + varGToParent < _cacheArray[i][j]._g)
                    {
                        // *如果是这样，就把这一格的父节点改成当前格，并且重新计算这一格的G和F值。
                        _cacheArray[i][j]._parentNode = currentNode;
                        _cacheArray[i][j]._g = currentNode->_g + varGToParent;
                        _cacheArray[i][j]._f = _cacheArray[i][j]._g + _cacheArray[i][j]._h;
                    }
                }
                else
                {
                    // * 如果它不在开启列表中
                    // *把它添加进去。
                    _openList.push_back( &_cacheArray[i][j]);
                    
                    // *把当前格作为这一格的父节点。
                    _cacheArray[i][j]._parentNode = currentNode;
                    
                    // *记录这一格的F,G,和H值。
                    // G
                    int varGToParent;
                    if (i == currentNode->_i || j == currentNode->_j)
                        varGToParent=HORIZONTAL_VERTICAL_COST;
                    else
                        varGToParent = DIAGONAL_COST;
                    _cacheArray[i][j]._g= currentNode->_g + varGToParent;
                    
                    // H
                    _cacheArray[i][j]._h = abs( i - dest_i ) * HORIZONTAL_VERTICAL_COST + abs( j - dest_j ) * HORIZONTAL_VERTICAL_COST;
                    
                    // F
                    _cacheArray[i][j]._f = _cacheArray[i][j]._g + _cacheArray[i][j]._h;
                    
                }
            }
        }
    }
    
    // 3 输出路径
    if (!isSucceed)
    {
        // 未找到路径
        
        // 开启使用近似点目标点
        if (_isUseApproximateDest)
        {
            ASNode * approximateDestNode = _closeList[0];
            for (unsigned int i = 1; i < _closeList.size(); i++)
            {
                if (approximateDestNode->_h == 0 && _closeList[i]->_h > 0)
                {
                    approximateDestNode = _closeList[i]; // XXX 如果h==0，就强行覆盖，防止出错
                }
                else if (_closeList[i]->_h < approximateDestNode->_h)
                {
                    approximateDestNode = _closeList[i];
                }
            }
            
            return createPath(approximateDestNode, src_i, src_j);
            
        }
        else
        {
            return vector<ASNode>(); 
        }       
    }
    else
    {
        return createPath( &_cacheArray[dest_i][dest_j],src_i,src_j);
    }
    
}

vector<ASNode> AStar::createPath( ASNode *nearNode ,int src_i,int src_j)
{
    vector<ASNode> path;
    
    path.push_back(*nearNode);
    
    ASNode *nextNode=nearNode->_parentNode;
    
    do 
    {
        path.push_back(*nextNode);
        
        if (nextNode == &_cacheArray[src_i][src_j])
        {
            break;
        }
        
        nextNode = nextNode->_parentNode;
        
    } while (true);
    
    reverse(path.begin(), path.end());
    
    return path;
}



int main(){
    int **map = new int*[10 * 10];
    memset(map, 0, 10 * 10);

    AStar* astar = AStar::create(map,10,10);
    vector<ASNode> path = astar->findPath(0,1,9,9);
    astar->printPath(path);
    return 1;
}