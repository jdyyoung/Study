

20210706:

关键字：using  理解与使用

C++11中using 的使用_积累点滴，保持自我-CSDN博客 - https://blog.csdn.net/qq_22642239/article/details/102720404

20210630:

```
std::shared_ptr<IAacSource::frame_t>frm
```

//智能指针   模板

```
UINT64 frameFstPts, frameCurPts;
std::chrono::steady_clock::time_point pts;

//得出是系统上电就开始的时间
auto value = std::chrono::time_point_cast<std::chrono::milliseconds>(vfrm->pts);
frameFstPts = value.time_since_epoch().count();
打印出来：value 是string，frameFstPts 是uint64
------idx:0size:19744pts:61936ms //value打印出带ms:61936ms
[---121-----frameFstPts = 61936-------discard_frame_cnt = 0 ----]
```

(3条消息) std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();_代码实践-CSDN博客 - https://blog.csdn.net/qq_21950671/article/details/92106906

(3条消息) struct timespec 和 struct timeval_liu44235的专栏-CSDN博客_struct timespec - https://blog.csdn.net/liu44235/article/details/37692635

```
//C++ 11 线程里面延时5S
std::this_thread::sleep_for(5_s); 
//C++ 11 线程里面延时1ms
std::this_thread::sleep_for(1_ms);
```

(3条消息) c++中using的用法_YoungYangD的博客-CSDN博客_c++ using - https://blog.csdn.net/weixin_39640298/article/details/84641726

2021-0605：

day04

1.this指针

2.const 对象与const 函数

3.析构函数

4.拷贝构造函数

5.静态成员：静态成员变量与静态成员函数



单例模式 | 菜鸟教程 - https://www.runoob.com/design-pattern/singleton-pattern.html