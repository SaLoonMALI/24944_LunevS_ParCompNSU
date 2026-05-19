# Sam Lunev. 2026. All Rights Reserved.
import logging
import time
import cv2
import os
import queue
import threading


def sensor_worker(sensor, data_q, stop_event):
    while not stop_event.is_set():
        data = sensor.get()
        if data is not None:
            if not data_q.empty():
                try:
                    data_q.get_nowait()
                except queue.Empty:
                    pass
            data_q.put(data)

class Sensor:
    def get(self):
        logging.error("'get' undefined")
        raise NotImplementedError("'get' must be defined in subclass")
    
class SensorX(Sensor):
    def __init__(self, delay : float):
        self.delay = delay
        self._data = 0
        logging.info(f"SensorX init; Delay {self.delay}s")
    def __del__(self):
        logging.info(f"SensorX by the time {self.delay} freed")
    def get(self):
        time.sleep(self.delay)
        self._data+=1
        return self._data 
        
class WindowImage:
    def __init__(self, frequency):
        self.frequency = frequency
        self.last_update_time = 0
        logging.info(f"Images window start {self.frequency} FPS")
    def __del__(self):
        cv2.destroyAllWindows()
        logging.info("Images window closed")
    def show(self, image):
        cv2.imshow("Camera Feed", image)
        cv2.waitKey(self.frequency)


class SensorCam:
    def __init__(self, name, resolution):
        if(name):
            self.name = name
        else:
            logging.error("Cam Name Undefined")

        self.cap = cv2.VideoCapture(name)
        target_w, target_h = resolution
        
        if not self.cap.isOpened():
            logging.critical(f"Fatal: Cam {name} unfounded")
            return

        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, target_h)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, target_w)
        real_w = self.cap.get(cv2.CAP_PROP_FRAME_WIDTH)
        real_h = self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
    
    def __del__(self):
        if hasattr(self, 'cap') and self.cap.isOpened():
            self.cap.release()
            logging.info(f"CAM {self.name} freed")

    def get(self):
        if not self.cap.isOpened():
            logging.error("Err: closed device") 
            return None
        
        ret, frame = self.cap.read()

        if not ret:
            logging.error("Fatal: check CAM connection")
            return None

        return frame


if __name__ == "__main__":
    last_vals = [None, 0, 0, 0] 
    threads = []
    name_cam = 0
    resolution = (640, 480)
    fps = 30

    if not os.path.exists('log'): os.makedirs('log')
    logging.basicConfig(filename='log/app.log', level=logging.INFO, 
                        format='%(asctime)s - %(levelname)s - %(message)s')

    stop_event = threading.Event()
    
    cam = SensorCam(name_cam, resolution)
    s0 = SensorX(0.05) 
    s1 = SensorX(0.5)  
    s2 = SensorX(1.5)  
    all_sensors = [cam, s0, s1, s2]
    queues = [queue.Queue(maxsize=1) for _ in all_sensors]
    
    for s, q in zip(all_sensors, queues):
        t = threading.Thread(target=sensor_worker, args=(s, q, stop_event), daemon=True)
        t.start()
        threads.append(t)

    window = WindowImage(fps)

    try:
        while 1:
            for i, q in enumerate(queues):
                if not q.empty():
                    last_vals[i] = q.get()
            
            frame = last_vals[0]
            if frame is not None:
                display_frame = frame.copy()
                cv2.putText(display_frame, f"Sensor0: {last_vals[1]}", (10, 30), 1, 1.5, (0,255,0), 2)
                cv2.putText(display_frame, f"Sensor1: {last_vals[2]}", (10, 60), 1, 1.5, (0,255,0), 2)
                cv2.putText(display_frame, f"Sensor2: {last_vals[3]}", (10, 90), 1, 1.5, (0,255,0), 2)
                
                window.show(display_frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
    finally:
        stop_event.set()
