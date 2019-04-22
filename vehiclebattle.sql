/*
 Navicat Premium Data Transfer

 Source Server         : test
 Source Server Type    : MySQL
 Source Server Version : 80015
 Source Host           : localhost:3306
 Source Schema         : vehiclebattle

 Target Server Type    : MySQL
 Target Server Version : 80015
 File Encoding         : 65001

 Date: 22/04/2019 00:29:28
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account`  (
  `user_id` int(10) NOT NULL AUTO_INCREMENT,
  `username` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `password` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `experience` int(10) NOT NULL DEFAULT 0,
  `money` int(10) NOT NULL DEFAULT 0,
  `level` int(10) NOT NULL DEFAULT 0,
  `cur_vehicle_id` int(10) NOT NULL DEFAULT 1,
  `register_time` datetime(0) NULL DEFAULT NULL,
  `last_login_time` datetime(0) NULL DEFAULT NULL,
  PRIMARY KEY (`user_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 9 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account
-- ----------------------------
INSERT INTO `account` VALUES (3, 'test', '123123', 0, 0, 0, 1, '2019-04-19 22:10:22', '2019-04-22 00:27:40');

-- ----------------------------
-- Table structure for equipment
-- ----------------------------
DROP TABLE IF EXISTS `equipment`;
CREATE TABLE `equipment`  (
  `equipment_id` int(11) NOT NULL,
  `equipment_name` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `equipment_type` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `equipment_level` int(10) NOT NULL,
  `equipment_effect_value` int(10) NOT NULL,
  `equipment_duration` int(10) NOT NULL,
  `equipment_cooling_time` int(10) NOT NULL,
  `equipment_price` int(10) NOT NULL,
  `equipment_intro` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`equipment_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for possess_equipment
-- ----------------------------
DROP TABLE IF EXISTS `possess_equipment`;
CREATE TABLE `possess_equipment`  (
  `user_id` int(11) NOT NULL,
  `equipment_id` int(11) NOT NULL,
  `num` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`user_id`, `equipment_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for possess_vehicle
-- ----------------------------
DROP TABLE IF EXISTS `possess_vehicle`;
CREATE TABLE `possess_vehicle`  (
  `user_id` int(11) NOT NULL,
  `vehicle_id` int(11) NOT NULL,
  `equipment_ids` json NULL,
  PRIMARY KEY (`user_id`, `vehicle_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of possess_vehicle
-- ----------------------------
INSERT INTO `possess_vehicle` VALUES (3, 1, NULL);
INSERT INTO `possess_vehicle` VALUES (3, 2, NULL);
INSERT INTO `possess_vehicle` VALUES (3, 3, NULL);

-- ----------------------------
-- Table structure for vehicle
-- ----------------------------
DROP TABLE IF EXISTS `vehicle`;
CREATE TABLE `vehicle`  (
  `vehicle_id` int(11) NOT NULL,
  `vehicle_name` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `vehicle_type` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `vehicle_attack` float(10, 1) NOT NULL,
  `vehicle_motility` float(10, 1) NOT NULL,
  `vehicle_defend` float(10, 1) NOT NULL,
  `vehicle_health` float(10, 1) NOT NULL,
  `vehicle_price` float(10, 2) NOT NULL,
  `vehicle_intro` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`vehicle_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of vehicle
-- ----------------------------
INSERT INTO `vehicle` VALUES (1, '装甲车1', 'panzer', 2.5, 3.0, 2.5, 100.0, 10000.00, '默认装甲车，性能较差');
INSERT INTO `vehicle` VALUES (2, '坦克1', 'tank', 3.0, 2.0, 4.0, 150.0, 15000.00, '初级坦克,性能较差');
INSERT INTO `vehicle` VALUES (3, '装甲车2', 'panzer', 4.0, 6.0, 4.0, 150.0, 23000.00, '中级装甲车,性能较好');
INSERT INTO `vehicle` VALUES (4, '坦克2', 'tank', 5.5, 4.0, 7.0, 180.0, 30000.00, '中级坦克,性能较好');
INSERT INTO `vehicle` VALUES (5, '装甲车3', 'panzer', 8.0, 10.0, 6.0, 180.0, 50000.00, '高级装甲车,性能很好');
INSERT INTO `vehicle` VALUES (6, '坦克3', 'tank', 10.0, 7.0, 10.0, 200.0, 80000.00, '高级坦克,性能很好');

SET FOREIGN_KEY_CHECKS = 1;
