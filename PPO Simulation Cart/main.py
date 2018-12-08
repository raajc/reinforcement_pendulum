#!/usr/bin/python3
import gym
import math
import numpy as np
import tensorflow as tf
from policy_net import Policy_net
from ppo import PPOTrain
import PyDuino as PD
import serial
from pytictoc import TicToc
import time


ITERATION = int(1e5)
GAMMA = 0.95
ser = 0
SERIAL_AVAILABLE = True

t=TicToc()

def main():
    angle = 0.0
    angle_thres_deg = 15
    cart = 0.0
    t.tic()
    reward_max = 5
    reward_min = -5
    reward_disc = 5
    # Serial port for Arduino
    if (SERIAL_AVAILABLE):
        ser = serial.Serial('COM20', 115200)  # Initialize serial port
        print("connected to: " + ser.portstr)  # Confirm connection

    env = gym.make('CartPole-v0')
    env.seed(0)
    ob_space = env.observation_space
    Policy = Policy_net('policy', env)
    Old_Policy = Policy_net('old_policy', env)
    PPO = PPOTrain(Policy, Old_Policy, gamma=GAMMA)
    saver = tf.train.Saver()

    with tf.Session() as sess:
        writer = tf.summary.FileWriter('./log/train', sess.graph)
        sess.run(tf.global_variables_initializer()) #remove me if loading save
        obs = env.reset()
        reward = 0
        success_num = 0

        for iteration in range(ITERATION):  # episode
            observations = []
            actions = []
            v_preds = []
            rewards = []
            run_policy_steps = 0
            while True:  # run policy RUN_POLICY_STEPS which is much less than episode length
                run_policy_steps += 1
                obs = np.stack([obs]).astype(dtype=np.float32)  # prepare to feed placeholder Policy.obs
                act, v_pred = Policy.act(obs=obs, stochastic=True)

                act = np.asscalar(act)
                v_pred = np.asscalar(v_pred)

                observations.append(obs)
                actions.append(act)
                v_preds.append(v_pred)
                rewards.append(reward)

                # env.render()

                if (act == 1):
                    dir = "R";
                else:
                    dir = "L"

                if (SERIAL_AVAILABLE):
                    PD.writePWM(ser,180,dir)

                    last_angle = angle
                    angle_deg = PD.getPEncoderPos(ser)*360/1200 # convert encoder counts (1200) to degrees
                    angle = angle_deg * 2 * math.pi / 360 # convert degrees to radians

                    angle_velocity = (angle - last_angle)/t.tocvalue()

                    last_cart = cart
                    cart = PD.getMEncoderPos(ser)
                    cart_velocity = (cart - last_cart)/t.tocvalue()

                    #print("Angle {:.1f}, Angle_vel (rad/s) {:.1f}, Position (mm) {:.1f}, Velocity (mm/s) {:.1f}".format(angle, angle_velocity, cart,cart_velocity))

                    t.tic()

                m = (reward_max - reward_min) / (reward_disc - angle_thres_deg)
                # reward = min(m*(abs(angle_deg)-reward_disc) + reward_max, reward_max)
                #reward = 1
                reward = ((.9/7) * (min((6 - abs(angle_deg)), (1)))+6) + ((0.1/6)*(min((5 - abs((cart/1000))), (1)) + 5))

                # next_obs = [angle angle_velocity cart cart_velocoty]
                # print(next_obs)

                next_obs = [angle, angle_velocity, cart, cart_velocity]
                #print("angle = ", angle_deg)
                print("x: ", PD.getMEncoderPos(ser))
                if abs(angle_deg) > angle_thres_deg:
                    v_preds_next = v_preds[1:] + [0]  # next state of terminate state has 0 state value
                    print("reward: ", sum(rewards))
                    obs = env.reset()
                    reward = -1
                    print("Iteration: ", iteration)
                    print('Waiting to reset')
                    PD.writePWM(ser, 0, dir)
                    saver.save(sess, "./model/model_iter_{:d}_rewards_{:d}.ckpt".format(iteration, int(sum(rewards))))
                    print('Clear!! Model saved.')
                    while(angle_deg > 1 or angle_deg < -1):
                        time.sleep(0.1)
                        angle_deg = PD.getPEncoderPos(ser) * 360 / 1200
                    print('Entered iteration {:1f}'.format(iteration+1))
                    break
                else:
                    obs = next_obs

            writer.add_summary(tf.Summary(value=[tf.Summary.Value(tag='episode_length', simple_value=run_policy_steps)])
                               , iteration)
            writer.add_summary(tf.Summary(value=[tf.Summary.Value(tag='episode_reward', simple_value=sum(rewards))])
                               , iteration)

            if sum(rewards) >= 195:
                success_num += 1
                if success_num >= 100:
                    saver.save(sess, './model/model.ckpt')
                    print('Clear!! Model saved.')
                    break
            else:
                success_num = 0

            gaes = PPO.get_gaes(rewards=rewards, v_preds=v_preds, v_preds_next=v_preds_next)

            # convert list to numpy array for feeding tf.placeholder
            observations = np.reshape(observations, newshape=[-1] + list(ob_space.shape))
            actions = np.array(actions).astype(dtype=np.int32)
            rewards = np.array(rewards).astype(dtype=np.float32)
            v_preds_next = np.array(v_preds_next).astype(dtype=np.float32)
            gaes = np.array(gaes).astype(dtype=np.float32)
            if iteration > 0:
                gaes = (gaes - gaes.mean()) / gaes.std()

            PPO.assign_policy_parameters()

            inp = [observations, actions, rewards, v_preds_next, gaes]

            # train
            for epoch in range(4):
                sample_indices = np.random.randint(low=0, high=observations.shape[0], size=64)  # indices are in [low, high)
                sampled_inp = [np.take(a=a, indices=sample_indices, axis=0) for a in inp]  # sample training data
                PPO.train(obs=sampled_inp[0],
                          actions=sampled_inp[1],
                          rewards=sampled_inp[2],
                          v_preds_next=sampled_inp[3],
                          gaes=sampled_inp[4])

            summary = PPO.get_summary(obs=inp[0],
                                      actions=inp[1],
                                      rewards=inp[2],
                                      v_preds_next=inp[3],
                                      gaes=inp[4])[0]

            writer.add_summary(summary, iteration)
        writer.close()
        if (SERIAL_AVAILABLE):
            ser.close()


if __name__ == '__main__':
    main()

    # keyboard = input()
    # if keyboard.lower() in ['save', 'saver', 'saved']:
    #     saver.save(sess,
    #                "./model/model_iter_{:d}_rewards_{:d}.ckpt".format(iteration, sum(rewards)))
    #     print('Clear!! Model saved.')
    # elif keyboard.lower() in ['plot']:
    #     print('Do not have plot yet!')
    # elif keyboard.lower() in ['load']:
    #     saver.restore(sess,
    #                   "./model/model_iter_{:d}_rewards_{:d}.ckpt".format(iteration, sum(rewards)))
    #     writer = tf.summary.FileWriter('./log/train', sess.graph)
    #     print('Loaded new model!')
