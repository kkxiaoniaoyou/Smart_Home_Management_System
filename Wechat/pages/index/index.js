import mqtt from'../../utils/mqtt.js';
const aliyunOpt = require('../../utils/aliyun/aliyun_connect.js');

let that = null;

Page({
    data:{

      //设置温度值和湿度值 
      Temp:0,
      Wet:0,
      //humidity:"",
      LightSwitch:"",

      client:null,//记录重连的次数
      reconnectCounts:0,//MQTT连接的配置
      options:{
        protocolVersion: 4, //MQTT连接协议版本
        clean: false,
        reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
        connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
        resubscribe: true, //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
        clientId: '',
        password: '',
        username: '',
      },

      aliyunInfo: {
        productKey: 'k21dtycd3Rr', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        deviceName: 'WeChatDUAN2', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        deviceSecret: 'eb0653a79b9ea091df1484c7112fac7c', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        regionId: 'cn-shanghai', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        pubTopic: '/k21dtycd3Rr/WeChatDUAN2/user/WeChatDUAN2', //发布消息的主题
        subTopic: '/k21dtycd3Rr/WeChatDUAN2/user/get', //订阅消息的主题
      },
    },

  onLoad:function(){
    that = this;
    let clientOpt = aliyunOpt.getAliyunIotMqttClient({
      productKey: that.data.aliyunInfo.productKey,
      deviceName: that.data.aliyunInfo.deviceName,
      deviceSecret: that.data.aliyunInfo.deviceSecret,
      regionId: that.data.aliyunInfo.regionId,
      port: that.data.aliyunInfo.port,
    });

    console.log("get data:" + JSON.stringify(clientOpt));
    let host = 'wxs://' + clientOpt.host;
    
    this.setData({
      'options.clientId': clientOpt.clientId,
      'options.password': clientOpt.password,
      'options.username': clientOpt.username,
    })
    console.log("this.data.options host:" + host);
    console.log("this.data.options data:" + JSON.stringify(this.data.options));

    //访问服务器
    this.data.client = mqtt.connect(host, this.data.options);

    this.data.client.on('connect', function (connack) {
      wx.showToast({
        title: '连接成功'
      })
      console.log("连接成功");
    })

    //接收消息监听
    that.data.client.on("message", function (topic, payload) {
      //message是一个16进制的字节流
      let dataFromALY = {};
      try {
        dataFromALY = JSON.parse(payload);
        console.log(dataFromALY);
       that.setData({
        //转换成JSON格式的数据进行读取
        LightSwitch:dataFromALY.LightSwitch,
        Temp:dataFromALY.Temp,
        Wet:dataFromALY.Wet,
      })
      } catch (error) {
        console.log(error);
      }
    })

    //服务器连接异常的回调
    that.data.client.on("error", function (error) {
      console.log(" 服务器 error 的回调" + error)

    })
    //服务器重连连接异常的回调
    that.data.client.on("reconnect", function () {
      console.log(" 服务器 reconnect的回调")

    })
    //服务器连接异常的回调
    that.data.client.on("offline", function (errr) {
      console.log(" 服务器offline的回调")
    })
  },
  onClickTemp() {
    that.sendTemp(this.data.Temp);
  },
  onClickWet() {
    that.sendWet(this.data.Wet);
  },
  onClickOpen() {
    that.sendLight("on");
  },
  onClickOff() {
    that.sendLight("off");
  },
  sendWet(data) {
    let sendData = {
      Wet: data,
    }
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish(this.data.aliyunInfo.pubTopic, JSON.stringify(sendData));
      console.log(this.data.aliyunInfo.pubTopic)
      console.log(JSON.stringify(sendData))
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },
  sendTemp(data) {
    let sendData = {
      Temp: data,
    }
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish(this.data.aliyunInfo.pubTopic, JSON.stringify(sendData));
      console.log(this.data.aliyunInfo.pubTopic)
      console.log(JSON.stringify(sendData))
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },
  sendLight(data) {
    let sendData = {
      LightSwitch: data,
    };

    //发布消息
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish(this.data.aliyunInfo.pubTopic, JSON.stringify(sendData));
      console.log(this.data.aliyunInfo.pubTopic)
      console.log(JSON.stringify(sendData))
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
    //此函数是订阅的函数，因为放在访问服务器的函数后面没法成功订阅topic，因此把他放在这个确保订阅topic的时候已成功连接服务器
//订阅消息函数，订阅一次即可 如果云端没有订阅的话，需要取消注释，等待成功连接服务器之后，在随便点击（开灯）或（关灯）就可以订阅函数
    //  this.data.client.subscribe(this.data.aliyunInfo.subTopic,function(err){
    //   if(!err){
    //     console.log("订阅成功");
    //   };
    //   wx.showModal({
    //     content: "订阅成功",
    //     showCancel: false,
    //   })
    // })  
    
  }
})