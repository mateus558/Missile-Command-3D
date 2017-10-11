//
// Created by mateus558 on 25/08/17.
//

#include "Random.h"

#include <random>
#include <vector>

namespace Random {
    std::minstd_rand gen;
	
    void init() {
        gen.seed(std::random_device{} ());
    }

    int intInRange(int low, int high) {
        std::uniform_int_distribution<int> dist(low, high);

        return dist(gen);
    }

    float floatInRange(float low, float high) {
        std::uniform_real_distribution<float> dist(low, high);

        return dist(gen);
    }
    
	std::vector<int> randomArray(int n){
		int i, j, temp;
		std::vector<int> arr(n+1);
		
		for(i = 0; i <= n; i++){
			arr[i] = i;
		}
		
		for(i = 0; i <= n; i++){
			j = intInRange(0, n);
			temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
		}	
		
		return arr;
	}    
}
