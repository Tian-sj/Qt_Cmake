CREATE DATABASE IF NOT EXISTS FreqStation;

USE FreqStation;

CREATE TABLE IF NOT EXISTS t_UserInfo (
    UserInfo_Id INT AUTO_INCREMENT PRIMARY KEY COMMENT '用户唯一标识',
    UserInfo_UserName VARCHAR(20) NOT NULL UNIQUE COMMENT '用户名，唯一',
    UserInfo_Password VARCHAR(20) NOT NULL COMMENT '用户密码',
    UserInfo_Role ENUM('Admin', 'Operator', 'Authorizer') NOT NULL COMMENT '用户角色（管理员或操作员或授权员）',
    UserInfo_Nickname VARCHAR(50) NOT NULL COMMENT '用户昵称',
    UserInfo_CreationTime VARCHAR(23) NOT NULL COMMENT '用户创建时间（字符串类型）'
);

CREATE TABLE t_UserRealTimeData (
    UserRealTimeData_Id INT AUTO_INCREMENT PRIMARY KEY COMMENT '实时数据唯一标识',
    UserInfo_Id INT NOT NULL COMMENT '用户ID',
    UserRealTimeData_Value JSON NOT NULL COMMENT '存储具体数据值，如幅值、相角等',
    UserRealTimeData_CreationTime VARCHAR(23) NOT NULL COMMENT '数据更新时间（字符串类型）',
    FOREIGN KEY (UserInfo_Id) REFERENCES t_UserInfo(UserInfo_Id)
);

CREATE INDEX idx_userinfo_creationtime ON t_UserRealTimeData(UserInfo_Id, UserRealTimeData_CreationTime DESC);

CREATE TABLE t_UserFrequencyTests (
    UserFrequencyTests_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '测试记录唯一标识',
    UserInfo_Id INT NOT NULL COMMENT '用户ID',
    UserFrequencyTests_Type ENUM('FixedFrequency', 'FrequencyCurve') NOT NULL COMMENT '测试类型（固定频率或频率曲线）',
    UserFrequencyTests_StartTime VARCHAR(23) NOT NULL COMMENT '测试开始时间（字符串类型）',
    UserFrequencyTests_EndTime VARCHAR(23) COMMENT '测试结束时间（字符串类型）',
    UserFrequencyTests_Result VARCHAR(255) NOT NULL COMMENT '测试结果，存储测试数据',
    UserFrequencyTests_RecordingFilePath VARCHAR(255) COMMENT '录波文件存储路径',
    FOREIGN KEY (UserInfo_Id) REFERENCES t_UserInfo(UserInfo_Id)
);

CREATE TABLE t_UserAssessmentResults (
    UserAssessmentResults_Id INT AUTO_INCREMENT PRIMARY KEY COMMENT '考核结果唯一标识',
    UserInfo_Id INT NOT NULL COMMENT '用户ID',
    UserAssessmentResults_Name VARCHAR(255) NOT NULL COMMENT '机组名称',
    UserAssessmentResults_Result JSON NOT NULL COMMENT '考核结果，存储为JSON格式',
    UserAssessmentResults_CreationTime VARCHAR(23) NOT NULL COMMENT '考核结果生成时间（字符串类型）',
    FOREIGN KEY (UserInfo_Id) REFERENCES t_UserInfo(UserInfo_Id)
);

CREATE TABLE t_UserCommunicationEvents (
    UserCommunicationEvents_Id INT AUTO_INCREMENT PRIMARY KEY COMMENT '通信事件唯一标识',
    UserInfo_Id INT NOT NULL COMMENT '用户ID',
    UserCommunicationEvents_Type VARCHAR(255) NOT NULL COMMENT '事件类型，如通信成功、失败等',
    UserCommunicationEvents_Description TEXT COMMENT '事件描述',
    UserCommunicationEvents_CreationTime VARCHAR(23) NOT NULL COMMENT '事件记录时间（字符串类型）',
    FOREIGN KEY (UserInfo_Id) REFERENCES t_UserInfo(UserInfo_Id)
);

CREATE TABLE t_UserOperationLogs (
    UserOperationLogs_Id INT AUTO_INCREMENT PRIMARY KEY COMMENT '操作日志唯一标识',
    UserInfo_Id INT NOT NULL COMMENT '用户ID',
    UserOperationLogs_Action VARCHAR(255) NOT NULL COMMENT '操作描述',
    UserOperationLogs_Details TEXT COMMENT '操作详情',
    UserOperationLogs_CreationTime VARCHAR(23) NOT NULL COMMENT '日志记录时间（字符串类型）',
    FOREIGN KEY (UserInfo_Id) REFERENCES t_UserInfo(UserInfo_Id)
);

CREATE TABLE t_UserAlarmLogs (
    UserAlarmLogs_Id INT AUTO_INCREMENT PRIMARY KEY COMMENT '告警记录唯一标识',
    UserInfo_Id INT NOT NULL COMMENT '用户ID',
    UserAlarmLogs_Type VARCHAR(255) NOT NULL COMMENT '告警类型',
    UserAlarmLogs_Description TEXT COMMENT '告警描述',
    UserAlarmLogs_Status ENUM('Resolved', 'Unresolved') NOT NULL COMMENT '告警状态（已解除、未解除）',
    UserAlarmLogs_CreationTime VARCHAR(23) NOT NULL COMMENT '告警记录时间（字符串类型）',
    FOREIGN KEY (UserInfo_Id) REFERENCES t_UserInfo(UserInfo_Id)
);