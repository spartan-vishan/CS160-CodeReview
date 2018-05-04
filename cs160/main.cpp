//
//  main.cpp
//  cs160
//
//  Created by Vishan Menon on 5/3/18.
//  Copyright © 2018 Vishan Menon. All rights reserved.
//

#include <iostream>
#include "main.h"

using namespace std;

int main(int argc, char** argv) {

   cout << "Begin landmark detection" << endl;
   clock_t t1 = clock();
   landmark_data *imgLandmarks;
   imgLandmarks = new landmark_data[argc];
   pthread_t threads[NUM_THREADS];
   thread_data td[NUM_THREADS];
   pthread_attr_t attr;
   void *status;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   int rc;

   int imgPerThread = (argc - 1) / NUM_THREADS;

   for (int i = 0; i < NUM_THREADS; i++) {
      td[i].images = argv;
      //td[i].numImages = argc;
      td[i].numImages = argc - 1;
      td[i].start_idx = (i * imgPerThread) + 1;

      //if (i == 0) {td[i].start_idx = 1;}
      //else {td[i].start_idx = i * ((argc - 1) / NUM_THREADS);}

      td[i].end_idx = ((i + 1) * (imgPerThread)) + 1;

      //cout << "val: " << i << endl;
      //cout << td[i].start_idx << endl;
      //cout << td[i].end_idx << endl;

      //td[i].end_idx = (i + 1) * ((argc - 1) / NUM_THREADS) - 1;
      td[i].landmarkData = imgLandmarks;

      rc = pthread_create(&threads[i], &attr, getLandmarksThread, (void *)&td[i]);

      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   pthread_attr_destroy(&attr);

   for(int i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_join(threads[i], &status);

      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }

      cout << "Main: completed thread id :" << i ;
      cout << "  exiting with status :" << status << endl;
   }

   cout << "End landmark detection" << endl;
   clock_t t2 = clock();

   cout << "Begin drawing triangles" << endl;
   drawDelauneyTrianglesThread(imgLandmarks, argv, argc);
   delete [] imgLandmarks;
   clock_t t3 = clock();
   cout << "End drawing triangles" << endl;

   double dLibTime = (double) (t2 - t1) / CLOCKS_PER_SEC * 1000.0;
   double openCVTime = (double) (t3 - t2) / CLOCKS_PER_SEC * 1000.0;

   cout << "DLib execution time " << dLibTime << endl;
   cout << "OpenCV execution time " << openCVTime << endl;

   return 0;
}
