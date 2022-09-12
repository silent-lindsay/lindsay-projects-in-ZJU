```shell
#执行shell脚本
wanghui@vmware:~$ ./hello.sh
Hello,world!
wanghui@vmware:~$ /bin/sh hello.sh
Hello,world!
wanghui@vmware:~$ /home/wanghui/hello.sh
Hello,world!
wanghui@vmware:~$ hello.sh
hello.sh：未找到命令
```

运行时必须加上路径，`./`表示当前路径，或者写出绝对路径

###### 第一个shell脚本程序

###### $第一行添加解释器路径$

```shell
#！/bin/sh 
echo "Hello,world!"
```

###### 权限

必须有x权限才能执行

命令行查看权限：```ls -l```

权限格式：```-rwxrwxr-x ```

命令行更新权限：

```chmod +x hello.sh``` 添加执行权限

```chmod -x hello.sh``` 去除执行权限

###### 变量

- **def** `NAME=VALUE` *等号左右不加空格*

- **use** `${NAME}` 有定界符可简写为 `$NAME` 

	例如： `echo $NAME/build/`

###### 环境变量

```shell
#define 
export OUTDIR=/opt/
#call
echo ${OUTdIR}
```

查看所有环境变量 ：`printenv`

