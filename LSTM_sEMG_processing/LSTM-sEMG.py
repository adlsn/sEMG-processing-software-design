#coding:utf-8
__author__ = 'jmh081701'
import numpy as np
import tensorflow as tf
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
# from tensorflow.examples.tutorials.mnist import  input_data
# from tensorflow.contrib import rnn

# mnist=input_data.read_data_sets("./data",one_hot=True)
# mnist = np.loadtxt(r"C:\\Users\ZQC\Desktop\fdata.txt")   #将文件中数据加载到data数组里


train_rate = 0.05 # 学习速率
train_step = 350
batch_size = 8  # 每批样本数
display_step = 2

frame_size = 1     # 序列中每一个分量的大小
sequence_length = 8  # 每个样本序列的长度
hidden_num = 5  # 隐层个数
n_classes = 7 # 类别

#读入X数据
dataxtrain = np.loadtxt(r"G:\\最近工作文档\下肢康复外骨骼\-----毕业论文\肌电采集实验\运动模式识别程序\tensorflow\xtrain3000.txt")   #将文件中数据加载到datax数组里
dataxtest = np.loadtxt(r"G:\\最近工作文档\下肢康复外骨骼\-----毕业论文\肌电采集实验\运动模式识别程序\tensorflow\xtest800.txt")   #将文件中数据加载到datax数组里

#读入y数据
dataytrain = np.loadtxt(r"G:\\最近工作文档\下肢康复外骨骼\-----毕业论文\肌电采集实验\运动模式识别程序\tensorflow\ytrain3000.txt")   #将文件中数据加载到datay数组里
dataytest= np.loadtxt(r"G:\\最近工作文档\下肢康复外骨骼\-----毕业论文\肌电采集实验\运动模式识别程序\tensorflow\ytest800.txt")   #将文件中数据加载到datay数组里

# print(dataxtrain.shape) 输出数组维度

outputy = tf.one_hot(dataytrain, n_classes)
outputytest = tf.one_hot(dataytest, n_classes)



# 定义单个样本输入,输出
x = tf.placeholder(dtype=tf.float32, shape=[None,sequence_length*frame_size],name="inputx")
y = tf.placeholder(dtype=tf.int64, shape=[None,n_classes],name="expected_y")

# 定义权值
weights=tf.Variable(tf.truncated_normal(shape=[hidden_num,n_classes]))  # 最后一层权重
bias=tf.Variable(tf.zeros(shape=[n_classes]))  # 最后一层偏置


def rnn(xx, weights, bias):
    xx = tf.reshape(xx, shape=[-1, sequence_length, frame_size])
    rnn_cell = tf.nn.rnn_cell.BasicRNNCell(hidden_num)
    # init_state = tf.zeros(shape=[batch_size, rnn_cell.state_size])
    # 其实这是一个深度RNN网络,对于每一个长度为n的序列[x1,x2,x3,...,xn]的每一个xi,都会在深度方向跑一遍RNN,跑上hidden_num个隐层单元
    output, states = tf.nn.dynamic_rnn(rnn_cell, xx, dtype=tf.float32)
    return tf.nn.softmax(tf.matmul(output[:, -1, :], weights)+bias, 1)  # tf.matmul（）将矩阵a乘以矩阵b，生成a * b。

# 经过RNN后预测值
predy = rnn(x, weights, bias)

# 定义损失函数
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=predy, labels=y))

# 定义优化函数
train = tf.train.AdamOptimizer(train_rate).minimize(cost)

correct_pred = tf.equal(tf.argmax(predy, 1), tf.argmax(y, 1))   # argmax返回的是最大数的索引 0每列最大值，1每行最大值
# 定义精确度
accuracy = tf.reduce_mean(tf.to_float(correct_pred))

sess = tf.Session()
sess.run(tf.global_variables_initializer())   # 初始化变量
dataytrain = sess.run(outputy)
dataytest = sess.run(outputytest)
step = 0

# testx, testy = mnist.test.next_batch(batch_size)


epochstep =1
# 循环训练
# while epochstep < 10 :
#
#
#     epochstep += 1
acc = 0
loss = 0
step = 0
while step < train_step:
    # batch_x, batch_y = mnist.train.next_batch(batch_size)
    # start = step * 8  # train 3000 个一圈
    # batch_x = dataxtrain[start:start+8]
    # batch_y = dataytrian[start:start+8]

    iter = step * batch_size
    itertest = iter % 800
    batch_x = dataxtrain[iter:iter + batch_size]
    batch_y = dataytrain[iter:iter + batch_size]
    #    batch_x=tf.reshape(batch_x,shape=[batch_size,sequence_length,frame_size])
    _loss, __ = sess.run([cost, train], feed_dict={x: batch_x, y: batch_y})
    # testx = dataxtest[start:start + 8]
    # testy = dataytest[start:start + 8]
    testx = dataxtest[0:800]
    testy = dataytest[0:800]

    if (step % display_step == 0 and itertest != 0):
        # acc, loss = sess.run([accuracy, cost], feed_dict={x: testx, y: testy})
        tempacc, temploss = sess.run([accuracy, cost], feed_dict={x: testx, y: testy})
        # accp = acc / 800
        # lossp = loss / 800
        print(step, tempacc, temploss)

    step += 1


