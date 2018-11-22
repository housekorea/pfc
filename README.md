# PFC(Personal Food Computer) by KgwangHee Han.


<p align="center">
  <img width="400" src="https://github.com/housekorea/pfc/blob/master/doc/pfc_v2_diykit.jpeg">
  <h3 align="center">Recent PFC DIY KIT(V2)</h3>
</p> 
<p align="center">
  <img width="400" src="https://github.com/housekorea/pfc/blob/master/doc/pfc_v3.jpg">  
  <h3 align="center">Original PFC V1</h3>
</p>

This repository's purpose is to distribute others who have interest in building there own PFC. It was first developed by MIT Media Lab Open Inititiative. Thanks to their efforts, I can start this exciting project(Along with my belief that we also have a possibility to beacome #NerdFarmer!). If you want to make your own PFC, you can absolutely fork this repsitory.(I hope that this repository is helpful!) 

Although hardware & software used in my personal PFC prototype and original prototype from MIT may differ, it is challengable to localize it in S.Korea for general use.   

I believe that PFC project is a huge step for urbanizing food production.  Like our NerdFarmers, farming with Information Technology rather than shovel and pick.

This repository have four directories.(It is still in development and I will summarize the philosophy and mechanism whenever I have time to write code.)   

<hr>

### 1. Sensors

 This directory contains codes for communication between Raspberry PI and Arduino through Serial Communication. It means that, codes in here is an ordering process to the arudino from Raspberry PI.(It does not include any directely related code for arduino sensors. See arduino directory for actual codes) 
 
**Sensors**

1. DHT11 (Air temperature, Air Humidity)
2. DS18B20 Water Proof sensor
3. CO2 Sensor
4. PhotoResistor(LDR)
5. EC(Electricity Conductivity) Sensor
6. PH Probe
7. Samsung USB camera
8. Water level sensor (Comming Soon)
9. Water Flow sensor (Comming Soon)

### 2. Actuators.
 
 This directory have codes related with "Actuators". In this text, the word "Actuators" means machine or any instrument that provides enviroment for plants to grow. For example, PFC simulates natural wind by using FANs, and provides light source using LED bulbs. For FANs and LEDs, we call it "Actuators".  
 
**Acutators**

1. LED
2. Air circulration FAN
3. Ventilation FAN
4. Air pump
5. Peristaltic Pump For Solution A
6. Peristaltic Pump For Solution B
7. Peristaltic Pump For Water
8. Peristaltic Pump For PH(-) (Comming Soon)
9. Peristaltic Pump For PH(+) (Comming Soon)
10. Solenoid Valve for each Peristaltic Pump (Considerable)
11. Chiller Unit (Considerable)

### 3. Computer Vision.

![Capture Shots](https://github.com/housekorea/pfc/blob/master/doc/2018-08-16_22:20:01_13_COL_KCL_CV.jpg) 
![Capture Shots](https://github.com/housekorea/pfc/blob/master/doc/2018-08-16_22:20:01_13_TRANSITION_IMAGES.jpg) 


 I have great expectations about the role of **"Computer Vision"**, it gives an opportunity on various applications ranging from recognizing symptoms for disease to measuring crops size or status. At this time I used openCV3 library with python2 programing language. You can check it in my sourcecodes that image processing uses OpenCV library to detect plant and measure their size. Also our openAG forum opened a specific thread to discuss about how to make this image processing plays important role of our PFC model. You can check various and great ideas of usecases of "Computer Vision" by visiting our therad 
 Probably, on my PFC's computer vision headed to the next stage for Machine Learning more specialize Image Processing. I hope that a number of smart guys joined this Computer Vision thread, and share their knowledge and experience through cooperation.

Link to our Computer Vision Thread on openAG forum : http://forum.openag.media.mit.edu/t/cv-and-image-processing-for-the-pfc/273/18

### 4. Arduino Code directory.

 Do you want to find arduino codes for making PFC? Explore "Arduino" Directory, this directory is the only space in our project for mananging Arduino Sourcecodes. Although I warn you about the pinout on my sourcodes. As of right now, I'm using "Arduino Mega 2560" microcontroller board Model and this sourcode is specialized for this specific microcontroller board. Pin numbers of GPIO(General Purpose Input/Output Pin) are listed in "Constant Variable" on my Arduino Sourcecode. 
 
 After that, I would like to write about the electronics design circuit about my PFC : ). I'm absolutely fighting with my inner laziness!

### 5. Database ERD

Data is the key of PFC project. Regardless of version or custom PFC your on, absoltuely next stage(we are headed) is Machine Learning through "Data". Before designing the Database, I have to keep flexibility and scalibility in mind. It means that, multiple PFCs can be operating in parrell at the same time. So it is possible that multiple PFCs may try to access remote database server where it can only have one address. Therefore I've designed a "Meta Table" of each sensor/actuator/particpant/models...etc. This meta table will have a key/value architecture, so that it could give us the flexibility but hard to using a wild joinning or exploring between meta tables.(I hope that a new "Search Technology" solving this problem. Such as ElasticSearch,Titan DB graph ...) 

**This ERD not finished for archiving PFC sensor data, it is still in development. I would love to hear any ideas or suggestions!**

![Initial Database Schema ERD](https://github.com/housekorea/pfc/blob/master/doc/house_PFC_v2.png) 

Any questions or suggestions is welcomed on my repository.


### AWS Service Architecture for PFC(In development)

<p align="center">
  <img width="600" src="https://github.com/housekorea/pfc/blob/master/doc/aws_pt.png">  
 <a href='https://www.youtube.com/watch?v=R8VVJ4uDW0I'><h3>AWS Summit Seoul 2018 Presention</h3></a>
</p>
<hr>

![Capture Shots](https://github.com/housekorea/pfc/blob/master/doc/aws%20service%20architecture.png) 


I considered AWS architecture for IoT feature in the PFC project. AWS has good features and many service protocols are related with machine learning. This is just a initial diagram to integrated AWS Architecture. through some experiment with AWS, I would modify and add it.


1. AWS IoT, Rule Engine

 Each PFC is considered as "Device" in the AWS IoT Context. Fortunately, they support Device SDK on the many programming languages(In my case I used python SDK on the Raspberry PI). This "Device SDK" specialized on "MQTT" Protocol and we can easily publish and subscribe the MQTT topic and message on AWS IoT Architecture. Rule Engine supports a number of ways to integrate service of AWS including Lambda, S3, DynamoDB or Machine Learning Service too. 

2. Lambda Function

 This Serverless Architecture, "Lambda Function" is a good feature to avoid managing the "Application Server". It only executes by the "Predefined rule on the Rule Engine" and "CloudWatch Event". This Lambda Function executes publish the "MQTT topic" on a specific event(or time). Along With that I expect this Lambda could support a lot of alternative server's roles, from the cronjob(Reserved schedule job) to Machine Learning Feature. 

3. IAM, Certificate, Policy

 I love this. AWS IoT strongly limits to take the "Security" to operate IoT Architecture Stack. We have to install individual Certificate and public/private key which is published by the AWS IoT. Absolutely this certificate base on the X.509. And "IAM", "Policy" would separate their role of the Each PFC or Manager's accessibility.

4. CloudWatch, ElasitcSearch

 CloudWatch and ELasticSearch support Analysis by the log and expand capacity of the management log on the PFC. In addition, CloudWatch support "Event" that has a very similar feature like a "Cron" on the Linux. I can modify scheduled job in the Cloud system. It is a very scalable way. Let me give an example, If I want to modify LED, Air Pump, Measuring Sensor data... Schedule of 10 PFC. Assume that if I managed this scheduled job on the cronjob in the Linux, I would have to modify individual PFC's cronjob(10 PFC). Whereas with CloudWatch I could just subscribe their job task on the cloud, and I only have to make one time modify "ColudWatch" event time, and all of the subscribed PFC changes their scheduled jobs. It is a very clean way of managing.

5. SES, SNS

 Notification feature in the AWS. If any abnormal symptom happens through the analysis process, alerts to the manager(or Farmer). 

6. DynamoDB

 NoSQL Database System to archive all of the sensors, actuators data

7. S3(Simple Storage Service)

 Object Bucket Service. Any captured images of the growing plant on the PFC gets saved in this bucket system. Any other "Blob" format data gets saved in here also. I expect Voice data and preprocessing image data generate by the OpenCV(PlantCV)

8. SageMaker

 New Feature. I have no experience just yet about this Machine Learning Feature, Though, I've heard that AWS team developed this feature nicely and their goal is to make this easy so that anybody without experties could use machine learning well. If SageMaker is not appropriate for my PFC project, I would use the "Spark ML" Library on the AWS. 

9. Lex, Poly

 It is related to the User Interface. It is just a plan as of right now. IoT demands more familiar and easy interaction methodology between device and human. If I want to control PFC by using my own voice, AWS supports their intelligence system "Alexa"("LEX" on the AWS). But currently it doesn't supports some languages like Korean. I contacted AWS architect on the S.Korea, But the only answer I got was that AWS team will open the Korean language in near future. But I don't know exactly when is the "near future".


<hr>
#NerdFarmer,

> KgwangHee Han(House)

> E-mail : house9737@gmail.com

Enjoying Farming guys!

**P.s 
If you are live in S.Korea, and you have a intereset in making a PFC in S.korea. Join Us through this Thread. I want contribute with others about the belows issue on our location(South Korea.)**

1. How to source alternative parts in S.kora local market.
2. How to break the language barrier of the documentations of MIT PFC's repository and fourm.(Until now, pretty much any PFC documentation is not translated in Korean. Probably it is one of the most anticipated activity for our Elementary, Middle schoole students to join PFC project.)
3. Solving problems origined by Local issue such as "Government Rule", "Local Traditional Culture", "Recogintion of Farmers". 

Doesn't matter what kind of issue we are facing, I have a optimistic about "We can solving this issue, and developing our agriculture on S.korea! Absolutely, we can helping others on the global too".


http://forum.openag.media.mit.edu/t/openag-in-south-korea/2801/16

(Above, Url link is a S.Korea Thread to talking about the any issue or communication in the S.Korea)

