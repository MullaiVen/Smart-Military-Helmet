import os
import cv2
import time
import numpy as np
import pandas as pd
from PIL import Image
#import tensorflow as tf
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib.patches as patches
import requests # request img from web
import shutil # save img locally
import time
import os
import numpy as np
import requests

probability_minimum = 0.5
threshold = 0.3


#Deeplearning_model:
weights1_path = 'Train/yolov3-helmet.weights'
configuration1_path = 'Train/yolov3-helmet.cfg'

network1 = cv2.dnn.readNetFromDarknet(configuration1_path, weights1_path)
layers_names1_all = network1.getLayerNames()
for i in network1.getUnconnectedOutLayers():
    print(i)
layers_names1_output = [layers_names1_all[i[0]-1] for i in network1.getUnconnectedOutLayers()]

labels1 = open('Train/helmet.names').read().strip().split('\n')
print('Data loaded:',labels1)


def Enemy_detection(file):
    image_input = cv2.imread(file)
    plt.rcParams['figure.figsize'] = (15.0,15.0)
    plt.imshow(cv2.cvtColor(image_input,cv2.COLOR_BGR2RGB))
    #plt.show()
    blob = cv2.dnn.blobFromImage(image_input,1/255.0,(416,416),swapRB=True,crop=False)
    blob_to_show = blob[0,:,:,:].transpose(1,2,0)
    network1.setInput(blob)
    output_from_network1 = network1.forward(layers_names1_output)
    np.random.seed(42)
    colours1 = np.random.randint(0,255,size=(len(labels1),3),dtype='uint8')

    print('Enemy: Helmet color:',colours1)

    bounding_boxes1 = []
    confidences1 = []
    class_numbers1 = []

    h,w = image_input.shape[:2]
                
    for result in output_from_network1:
        for detection in result:
            scores = detection[5:]
            class_current=np.argmax(scores)
            confidence_current=scores[class_current]
            if confidence_current>probability_minimum:
                box_current=detection[0:4]*np.array([w,h,w,h])
                x_center,y_center,box_width,box_height=box_current.astype('int')
                x_min=int(x_center-(box_width/2))
                y_min=int(y_center-(box_height/2))
                
                bounding_boxes1.append([x_min,y_min,int(box_width),int(box_height)])
                confidences1.append(float(confidence_current))
                class_numbers1.append(class_current)            

    results1 = cv2.dnn.NMSBoxes(bounding_boxes1,confidences1,probability_minimum,threshold)

    if len(results1) > 0:
        for i in results1.flatten():
            x_min,y_min=bounding_boxes1[i][0],bounding_boxes1[i][1]
            box_width,box_height= bounding_boxes1[i][2],bounding_boxes1[i][3]
            colour_box_current=[int(j) for j in colours1[class_numbers1[i]]]   
            th = cv2.rectangle(image_input,(x_min,y_min),(x_min+box_width,y_min+box_height),colour_box_current,5)
            
            cv2.rectangle(image_input,(x_min,y_min),(x_min+box_width,y_min+box_height),colour_box_current,5)
            text_box_current1='{}: {:.4f}'.format(labels1[int(class_numbers1[i])],confidences1[i])
            cv2.putText(image_input,text_box_current1,(x_min,y_min-7),cv2.FONT_HERSHEY_SIMPLEX,1.5,colour_box_current,5)
            #print(text_box_current1)
            print(text_box_current1)

    #print('Predicted-h:',labels1[int(class_numbers1[i])])
    if len(results1) > 0:
        print('Enemy detected')
    else:
        print('Object is not matching')

    #cv2.imshow('th',th)
    #cv2.waitKey(1000)
    plt.rcParams['figure.figsize'] = (15.0,15.0)
    #plt.imshow(cv2.cvtColor(image_input,cv2.COLOR_BGR2RGB))
    plt.savefig('Result/output.jpg')
    #plt.show()
def download():
    url = 'http://192.168.43.175/capture'
    file_name = 'Test/1.jpg'

    res = requests.get(url, stream = True)

    if res.status_code == 200:
        with open(file_name,'wb') as f:
            shutil.copyfileobj(res.raw, f)
        print('Image sucessfully Downloaded: ',file_name)
    else:
        print('Image Couldn\'t be retrieved')
if __name__ == '__main__':
    TestData="Test"
    while True:
        download()
        for(direcpath,direcnames,files) in os.walk(TestData):
            for file in files:
                if 'jpg' in file or 'jpeg' in file:
                    time.sleep(1)
                    filename= TestData +'/'+ file
                    print('Filename:',filename)
                    classification = Enemy_detection(filename)
                    os.remove(filename)
            time.sleep(5)





