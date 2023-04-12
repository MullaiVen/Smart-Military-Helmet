import pyautogui
import time
time.sleep(1)#120
while(True):
    myScreenshot = pyautogui.screenshot()
    myScreenshot.save(r'H:/code check/FinalProject(EnemyDetect)/Test/a.jpg')
    time.sleep(10)



