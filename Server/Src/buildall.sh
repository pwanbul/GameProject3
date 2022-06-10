echo "开始编译 ServerEngine"
cd ServerEngine && make -j2
cd ..
echo "开始编译 CenterServer"
cd CenterServer && make -j2
cd ..
echo "开始编译 GameServer"
cd GameServer && make -j2
cd ..
echo "开始编译 LogicServer"
cd LogicServer && make -j2
cd ..
echo "开始编译 LoginServer"
cd LoginServer && make -j2
cd ..
echo "开始编译 ProxyServer"
cd ProxyServer && make -j2
cd ..
echo "开始编译 AccountServer"
cd AccountServer && make -j2
cd ..
echo "开始编译 DBServer"
cd DBServer && make -j2
cd ..
echo "开始编译 LogServer"
cd LogServer && make -j2
cd ..
echo "编译结束，开始复制可执行文件"
cp ./CenterServer/CenterServer ./Linux/
cp ./GameServer/GameServer ./Linux/
cp ./LogicServer/LogicServer ./Linux/
cp ./LoginServer/LoginServer ./Linux/
cp ./ProxyServer/ProxyServer ./Linux/
cp ./AccountServer/AccountServer ./Linux/
cp ./DBServer/DBServer ./Linux/
cp ./LogServer/LogServer ./Linux/
echo "复制结束"

 
