# PFC(Personal Food Computer) by KgwangHee Han.


![Capture Shots](https://github.com/housekorea/pfc/blob/master/doc/KakaoTalk_Photo_2018-02-13-21-55-13_22.jpeg) 


This repository for purpose distribute to others who have a interests about the PFC. It first developed by MIT Media Lab Open Inititiative.Thanks to their efforts, i can started this exciting projects(Along with i believe that we have a possibility to beacame #NerdFarmer!).If you want make a PFC, absolutely you can fork this repsitory.(I hope that this repository helpful!) 

It is different some hardware/software between my personal PFC prototype and MIT original prototype. But it is challengable work to localizing in S.Korea for generalizng on here(S.Korea) 

This repository have a four types of directory.(Probably It would expanding now, whenever i have a time to writing about the more detail guide for PFC makers, i will summarize about the PFC and their philosophy and mechanism(with Vision too). 

I believe that PFC project is one of stepping stones for solving problem which we are facing about rapidly urbanziation and food. Our NerdFarmer, Farming using with a Information Technology than shoval and pick.

<hr>

### first, Sensor directory.

 This directory have a sourcecode to commnuicate with Rapsberry PI ans Arduion through Serial Communication. It means that, this codes more familiar with ordering to the arudino.(It is not include any directely relate code for arduino sensors. If you want check a code which relate in Arduino, check the Arduino Directory. 
 
**Sensor List**

1. DHT11 (Air temperature, Air Humidity)
2. DS18B20 Water Proof sensor
3. CO2 Sensor
4. PhotoResistor(LDR)
5. EC(Electricity Conductivity) Sensor
6. PH Probe
7. Samsung USB camera
8. Water level sensor (Comming Soon)
9. Water Flow sensor (Comming Soon)

### second, Actuator directory.
 
 This directory have a sourcecode related in "Actuators", In the my PFC project context, this word "Actuators" means machine or any instrument for making Enviroment of growing plants. Let me give an example, PFC simulate natural wind to using FAN cooler, and for giving light like a sun light, we usually using LED bulb. We can call this, "Actuators" for FAN,LED or anything which verifying purpose i mentioned. 
 
**Acutator List**

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
11. Chiler Unit (Considerable)

### thrid, Computer Vision directory.

 I have a great expectation about the role of **"Computer Vision"**, it is give a opportunity on the various ragne from  recognize about the any symptom for disease to measuring crops size,status too. In this time i used openCV3 library with python2 programing language. You can check my sourcecode relate this image processing for detecting plant and measuring their size using openCV library. At the same time, our openAG forum opend a specific thread to discuss for how to making this image processing to important role of our PFC model. You can check various and great ideas of usecase "Computer Vision". 
 Probably, on my PFC's computer vision headed to the next stage for Machine Learning more specialize Image Processing. I hope that a number of smart guys joined this Computer Vision thread, and sharing their knowledge and experience through cooperate.

Computer Vision Thread on openAG forum : http://forum.openag.media.mit.edu/t/cv-and-image-processing-for-the-pfc/273/18

### fourth, Arduino Code directory.

 If you want find arduino codes for making PFC? Exploring this "Arduino" Directory, this directory is only space for mananging Arduino Sourcecodes. I warned to you about the hardcode on my sourcodes. Until now, i used "Arduino Mega 2560" microcontroller board Model, this sourcode only specialized this specific microcontroller board. A number of GPIO(General Purpose Input/Output Pin) managed by "Constant Variable" on my Arduino Sourcecode. 
 
 After that, I would write about the electronics design circuit about my PFC codes : ). Absolutely i have to fight my inner laziness too!

### fifth, Database ERD

Data is key of PFC project, what kind of any version or custom of your PFC, absoltuely next stage(we are headed) is Machine Learning through "Data". For desing of Database, i have to consider some chracteristic about this project. It mean that, i have to aussme that parrell multiple PFC operate at the same time. But remote database server have a one address which other Prototype can be reach to this address, and it have a flexible and scalibility(i intended to make a "Meta Table of each sensor/actuator/particpant/models ...), this meta table have a key/value architecture, it would give us to flexsible but hard to using a wild joinning or exploring between meta tables.(I hope that a new "Search Technology" solving this problem. Such as ElasticSearch,Titan DB graph ...) 

**This ERD not finished for archiving PFC sensor data, it is continuing now, i would welcome any idea or suggestion!**

![Initial Database Schema ERD](https://github.com/housekorea/pfc/blob/master/doc/house_PFC_v2.png) 

Any question or suggestion welcomed on my repository.


### AWS Service Architecture for PFC(Developing)
![Capture Shots](https://github.com/housekorea/pfc/blob/master/doc/aws%20service%20architecture.png) 

I considered AWS architecture to include IoT feature in the PFC project. AWS has a good feature and many services relate in from the protocol to the machine learning. It is a just initial diagram to integrate AWS Architecture, through some experiment with AWS, I would modify and fitting it.


1. AWS IoT, Rule Engine

 Each PFC is "Device" in the AWS IoT Context. Fortunately, they support Device SDK on the many programming languages(In my case I used python SDK on the Raspberry PI). This "Device SDK" specialize "MQTT" Protocol and we can easily publish and subscribe the MQTT topic and message on AWS IoT Architecture. Rule Engine supports a number of ways to integrate service of AWS include Lambda, S3, DynamoDB or Machine Learning Service too. 

2. Lambda Function

 This Serverless Architecture, "Lambda Function" is a good feature to avoid manage the "Application Server". It only executed by the "Predefined rule on the Rule Engine" and "CloudWatch Event". This Lambda Function executes publish the "MQTT topic" on a specific event(or time). Along With, I expect this Lambda could support a lot of alternative server's role. From the cronjob(Reserved schedule job) to Machine Learning Feature. 

3. IAM, Certificate, Policy

 I love this. AWS IoT strongly limits to take the "Security" to operate IoT Architecture Stack. We have to install individual Certificate and public/private key which published by the AWS IoT. Absolutely this certificate base on the X.509. And "IAM", "Policy" would separate their role of the Each PFC or Manager's accessibility.

4. CloudWatch, ElasitcSearch

 CloudWatch and ELasticSearch support Analysis by the log and expand capacity the management log of the PFC. In addition, CloudWatch support "Event" has a very similar feature like a "Cron" on the Linux. I can modify scheduled job in the Cloud system. It is a very scalable way. Let me give an example, If I want to modify LED, Air Pump, Measuring Sensor data... Schedule of 10 PFC. Assume that if I managed this scheduled job on the cronjob in the Linux, I have to modify each all of the PFC cronjob(10 of the PFC). But they subscribe their job task on the cloud, I only one time modify "ColudWatch" event time, and all of the subscribe PFC changed their schedule jobs. It is a clear way.

5. SES, SNS

 Notification feature in the AWS. If finding the abnormal symptom through the analysis process alert to the manager(Farmer). 

6. DynamoDB

 NoSQL Database System to archive the all of the sensor, actuator data

7.S3(Simple Storage Service)

 Object Bucket Service. All of the images which captures the growing plant on the PFC would save in this bucket system. Any other "Blob" format data would save in here. I expect Voice data and preprocessing image data generate by the OpenCV(PlantCV)

8.SageMaker

 New Feature. Not yet I have no any experience this Machine Learning Feature. But, I heard that AWS team developed well this feature and they have a goal to the team who without data scientist could using machine learning well. If SageMaker is not appropriate well my PFC project, I would use the "Spark ML"Library on the AWS. 

9. Lex, Poly

 It relates to the User Interface. It just a plan. IoT demand more familiar and interaction methodology between device and human. I like an Order by the voice. AWS support their same intelligence system "Alexa"("LEX" on the AWS). But It has a local issue. Because of they  the Korean Language on the "LEX". I contact AWS architect on the S.Korea, They just say AWS team will open the Korean language in near future. But I don't know exactly what is "near future".


<hr>
#NerdFarmer,

> KgwangHee Han(House)

> E-mail : house9737@gmail.com

Enjoying Farming guys!

**P.s 
If you are live in S.Korea, and you have a intereset in making a PFC in S.korea. Join Us through this Thread. I want contribute with others about the belows issue on our location(South Korea.)**

1. How to sourcing alternative parts in S.kora local market.
2. How to break the language barrier of the documentation of MIT PFC's repository and fourm.(Until now, about any PFC documentation not translated in Korean language. Probably it is one of the valuable activity for our Elementary,Middle schoole students to joining PFC project.)
3. Solving problem origined by Local issue which such as "Government Rule", "Local Traditional Culture", "Recogintion of Farmers". 

Doesn't matter any issue which we are facing, i have a optimistic about the "We can solving this issue, and developing our agriculture on S.korea! Absolutely, we can helping others on the global too".


http://forum.openag.media.mit.edu/t/openag-in-south-korea/2801/16

(Above, Url link is a S.Korea Thread to talking about the any issue or communication in the S.Korea)

