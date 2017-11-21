
// Kitti tools
#include "libs/kitti/includes/kitti_point_cloud_reader.h"

#include "includes/core.h"
#include "includes/model_manager.h"
#include "includes/point_cloud_manager.h"

int main(int argc, char *argv[]){
  Toreo::Core window(argc, argv);

  // Point cloud handler
  Toreo::PointCloudManager PCM(&window);

  // Kitti point cloud reader
  KittiPointCloudReader point_cloud("/../KittiData/");
  boost::signals2::signal<void (unsigned int)> dataset_signal, frame_signal;
  point_cloud.connect_dataset(&dataset_signal);
  point_cloud.connect_frame(&frame_signal);

  dataset_signal(11);
  frame_signal(0);
//  point_cloud.connect_reader(window.signal_updated_screen());

  PCMid cloud_1 = PCM.add(point_cloud.point_cloud(), "Test #1", window.vehicle_frame(),
                          Visualizer::VARIABLE, true, 1.0f, 1.0f);
  PCM.translate(cloud_1, 0, 0, 1.73f);

  Toreo::ModelManager MM(&window);
  MMid db5_id = MM.load_new_model(DB5_BODY);
  MMelement car = MM.add(db5_id, window.vehicle_frame());
//  MM.metallize(db5_id, car, true, 1.0);
//  MM.roughen(db5_id, car, true, 0.6);

  return window.execute();
}
