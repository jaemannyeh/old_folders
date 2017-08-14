
#include "kwiz.h"

class Solution {
public:
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        for (unsigned int d1=0; d1<nums.size(); d1++) {
            for (unsigned int d2=d1+1; d2<nums.size(); d2++) {
                if (nums[d1]+nums[d2] == target) {
                    return NULL;
                }
            }
        }
        return NULL;
    }
};

int main(int argc, char *argv[]) {
    
    return 0;
}

