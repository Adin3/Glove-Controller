import pyautogui

class MouseMovement:
    def __init__(self):
        self.resolution = pyautogui.size()
        self.x, self.y = pyautogui.position()
        self.is_close_app = False
        pyautogui.moveTo(self.resolution[0] / 2, self.resolution[1] / 2)
    
    def change_min_time(self, time):
        pyautogui.MINIMUM_DURATION = time

    def move(self, posX, posY, time = 0):
        pyautogui.moveRel(posX, posY, time)

    def click(self):
        pyautogui.click()
