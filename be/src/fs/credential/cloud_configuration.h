// Copyright 2021-present StarRocks, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
#include <regex>

namespace starrocks {
class CloudCredential {
public:
    virtual ~CloudCredential() = default;
};

class HdfsCloudCredential final : public CloudCredential {
public:
    std::string authentication;
    std::string username;
    std::string password;
    std::string krbPrincipal;
    std::string krbKeyTabFile;
    std::string krbKeyTabData;

    //trim whitespace and single quotes from the begin and end of the string
    //example: " " -> ""
    //example: "'abc'" -> "abc"
    //example: "' abc '" -> "abc"
    std::string trim(const std::string & str)
    {
        if (str.empty()) return str;

        size_t start = 0;
        size_t end = str.length() - 1;

        // Trim leading whitespace and single quotes
        while (start <= end && (isspace(str[start]) || str[start] == '\'')) {
            start++;
        }

        // Trim trailing whitespace and single quotes
        while (end >= start && (isspace(str[end]) || str[end] == '\'')) {
            end--;
        }

        return str.substr(start, end - start + 1);
    }

    void parseFromString(std::string & hdfs_conf_str)
    {

        // 定位 HDFSCloudCredential 的起始和结束位置
        size_t start = hdfs_conf_str.find("HDFSCloudCredential{");
        if (start == std::string::npos) return ;
        start += std::string("HDFSCloudCredential{").length();

        size_t end = hdfs_conf_str.find("}", start);
        if (end == std::string::npos) return;

        // 提取 HDFSCloudCredential 的内容
        std::string credContent = hdfs_conf_str.substr(start, end - start);

        // 按行分割内容
        std::regex pattern("[=,]");
        std::sregex_token_iterator it(credContent.begin(), credContent.end(), pattern, -1);
        std::sregex_token_iterator iter_end;

        // 直接交替读取key-value对
        while (it != iter_end) {
            std::string key = trim(*it++);
            std::string value = trim(*it++);

            if (key == "authentication") {
                authentication = value;
            } else if (key == "username") {
                username = value;
            } else if (key == "password") {
                password = value;
            } else if (key == "krbPrincipal") {
                krbPrincipal = value;
            } else if (key == "krbKeyTabFile") {
                krbKeyTabFile = value;
            } else if (key == "krbKeyTabData") {
                krbKeyTabData = value;
            }
        }
    }
};

class AWSCloudCredential final : public CloudCredential {
public:
    bool use_aws_sdk_default_behavior;
    bool use_instance_profile;
    std::string access_key;
    std::string secret_key;
    std::string session_token;
    std::string iam_role_arn;
    std::string sts_region;
    std::string sts_endpoint;
    std::string external_id;
    std::string region;
    std::string endpoint;

    bool operator==(const AWSCloudCredential& rhs) const {
        return use_aws_sdk_default_behavior == rhs.use_aws_sdk_default_behavior &&
               use_instance_profile == rhs.use_instance_profile && access_key == rhs.access_key &&
               secret_key == rhs.secret_key && session_token == rhs.session_token && iam_role_arn == rhs.iam_role_arn &&
               sts_region == rhs.sts_region && sts_endpoint == rhs.sts_endpoint && external_id == rhs.external_id &&
               region == rhs.region && endpoint == rhs.endpoint;
    }
};

class AliyunCloudCredential final : public CloudCredential {
public:
    std::string access_key;
    std::string secret_key;
    std::string endpoint;

    bool operator==(const AliyunCloudCredential& rhs) const {
        return access_key == rhs.access_key && secret_key == rhs.secret_key && endpoint == rhs.endpoint;
    }
};

class CloudConfiguration {
public:
    virtual ~CloudConfiguration() = default;
};

class AWSCloudConfiguration final : public CloudConfiguration {
public:
    bool operator==(const AWSCloudConfiguration& rhs) const {
        return enable_path_style_access == rhs.enable_path_style_access && enable_ssl == rhs.enable_ssl &&
               aws_cloud_credential == rhs.aws_cloud_credential;
    }
    AWSCloudCredential aws_cloud_credential;
    bool enable_path_style_access = false;
    bool enable_ssl = true;
};

class AliyunCloudConfiguration final : public CloudConfiguration {
public:
    bool operator==(const AliyunCloudConfiguration& rhs) const {
        return aliyun_cloud_credential == rhs.aliyun_cloud_credential;
    }
    AliyunCloudCredential aliyun_cloud_credential;
};
} // namespace starrocks