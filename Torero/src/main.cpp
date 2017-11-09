
//kitti tools
#include "libs/kitti/includes/kitti_point_cloud_reader.h"

#include "includes/console.h"
#include "includes/point_cloud_manager.h"

#include <boost/signals2.hpp>

int main(int argc, char *argv[]){
  // Window handler
  Console GUI(argc, argv);
  // Point cloud handler
  PointCloudManager PCM(&GUI);

  boost::signals2::signal<void (unsigned int)> dataset_signal, frame_signal;

  KittiPointCloudReader point_cloud("/../KittiData/");
//  point_cloud.set_dataset(11);
//  point_cloud.goto_frame(0);
  point_cloud.connect_dataset(&dataset_signal);
  point_cloud.connect_frame(&frame_signal);

  dataset_signal(11);
  frame_signal(0);

  unsigned int cloud_1;
  cloud_1 = PCM.point_cloud_add(point_cloud.point_cloud(), "Test #1", GUI.vehicle_frame(),
                                Visualizer::VARIABLE, true, 1.0f, 1.0f);
  PCM.point_cloud_translate(cloud_1, 0, 0, 1.73f);

  // ----------------------------------------------------------- //
  // ------------------ THE COORDINATE SYSTEM ------------------ //
  // ----------------------------------------------------------- //
  //                                                             //
  //                               Z                             //
  //                               |     X                       //
  //                               |    /                        //
  //                               |   /                         //
  //                               |  /                          //
  //                               | /                           //
  //                               |/                            //
  //                 Y ––––––––––– o –––––––––––– -Y             //
  //                              /|                             //
  //                             / |                             //
  //                            /  |                             //
  //                           /   |                             //
  //                          /    |                             //
  //                        -X     |                             //
  //                              -Z                             //
  //                                                             //
  // ----------------------------------------------------------- //
  // ------------------ THE COORDINATE SYSTEM ------------------ //
  // ----------------------------------------------------------- //

  return GUI.execute();
}
