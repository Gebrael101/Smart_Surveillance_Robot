#include <gazebo/common/Plugin.hh>
#include <ros/ros.h>
#include <my_truck_description/Sound.h> // Corrected package name
#include <thread> // Include thread
#include <ros/callback_queue.h> // Include CallbackQueue

namespace gazebo
{
  class MicrophonePlugin : public ModelPlugin
  {
    // Constructor
    public: MicrophonePlugin() {}

    // Called when the plugin is loaded into the simulation
    public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Initialize ROS
      if (!ros::isInitialized())
      {
        int argc = 0;
        char **argv = NULL;
        ros::init(argc, argv, "microphone_plugin_node");
      }

      this->nodeHandle.reset(new ros::NodeHandle("microphone_plugin"));

      // Subscribe to the sound topic
      ros::SubscribeOptions so =
        ros::SubscribeOptions::create<my_truck_description::Sound>( // Corrected package name
        "/sound_topic",  // The topic to which your "microphone" is subscribed
        1,  // Queue size
        boost::bind(&MicrophonePlugin::OnSoundReceived, this, _1),
        ros::VoidPtr(),
        &this->rosQueue);

      this->rosSub = this->nodeHandle->subscribe(so);

      // Spin up the queue helper thread.
      this->rosQueueThread =
        std::thread(std::bind(&MicrophonePlugin::QueueThread, this));
    }

    // Handle incoming sound messages
    private: void OnSoundReceived(const my_truck_description::SoundConstPtr &msg) // Corrected package name
    {
      // Here you would implement logic to simulate sound detection
      ROS_INFO("Received sound with intensity: %f", msg->intensity);
    }

    // ROS helper function that processes messages
    private: void QueueThread()
    {
      static const double timeout = 0.01;
      while (this->nodeHandle->ok())
      {
        this->rosQueue.callAvailable(ros::WallDuration(timeout));
      }
    }

    // Pointer to the model
    private: physics::ModelPtr model;

    // Pointer to the node
    private: std::unique_ptr<ros::NodeHandle> nodeHandle;

    // Subscriber to the ROS topic
    private: ros::Subscriber rosSub;

    // A thread the keeps running the rosQueue
    private: std::thread rosQueueThread;

    // A ROS callbackqueue that helps process messages
    private: ros::CallbackQueue rosQueue;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(MicrophonePlugin)
}

