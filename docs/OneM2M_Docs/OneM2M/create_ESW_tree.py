from OneM2M import *
import time
import numpy as np

from team_list_file import team_list

server = "http://127.0.0.1:8080"
cse = "/~/in-cse/in-name/"

for i in range(47):
    ae = "Team" + str(i) + "_" + team_list[i]['title']
    print(ae)
    lbl = team_list[i]['labels']
    lbl = []
    register_ae(server+cse, ae, lbl)

    print(team_list[i]['nodes'])
    _ = [create_cnt(server+cse+ae, container_name) for container_name in team_list[i]['nodes']]

    content_instance = "test_instance"
    _ = [create_data_cin(server+cse+ae+"/"+container_name, content_instance)
         for container_name in team_list[i]['nodes']]

    time.sleep(0.3)
