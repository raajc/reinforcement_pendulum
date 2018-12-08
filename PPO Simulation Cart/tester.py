angle_deg = -15

angle_thres_deg = 15
reward_max = 5
reward_min = -5
reward_disc = 5
m = (reward_max - reward_min) / (reward_disc - angle_thres_deg)
reward = min(m * (abs(angle_deg) - reward_disc) + reward_max, reward_max)
# if angle_deg < reward_disc and angle_deg > -reward_disc:
#     reward = reward_max
# elif abs(angle_deg) < angle_thres_deg and abs(angle_deg) > reward_disc:
#     m = -(reward_max - reward_min)/(reward_disc-angle_thres_deg)
#     reward = m*(angle_deg-reward_disc) + reward_max
# else:
#     reward = reward_min
print(reward)