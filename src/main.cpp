#include <Arduino.h>
#include <vector>
#include <NeoPixelBus.h>
#include "Segment.hpp"
#include "RouteSegment.hpp"
#include "Route.hpp"
#include "Packet.hpp"

const uint16_t PixelCount = 100; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 4;  // make sure to set this to the correct pin, ignored for Esp8266
uint16_t pixel_run = 0;
uint16_t render_interval = 25;
uint16_t packet_interval = 1500;
uint16_t packet_next_interval = 50;
uint32_t last_packet_next = 0;
uint32_t last_packet = 0;
uint32_t last_render = 0;
uint8_t fade_length = 5;

std::vector<Packet> packets;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor backgroundColor[] = {RgbColor(0,0,16),RgbColor(8,0,0),RgbColor(255,0,180)};
RgbColor packetColor[] = {RgbColor(255,0,0),RgbColor(255,255,0),RgbColor(0,255,0),RgbColor(0,255,255),RgbColor(0,0,255),RgbColor(255,0,255)};

Segment segments[] = {
  Segment(7, backgroundColor[0],13),
  Segment(12, backgroundColor[1],20),
  Segment(7, backgroundColor[0],32),
  Segment(12, backgroundColor[1],39),
  Segment(7, backgroundColor[0],51),
  Segment(12, backgroundColor[1],58),
  Segment(7, backgroundColor[0],70),
  Segment(12, backgroundColor[1],77),
  Segment(13, backgroundColor[2],0)
};

std::vector<RouteSegment> route_vector[] = {
  {RouteSegment(segments[0]),RouteSegment(segments[1]),RouteSegment(segments[2],true)}, // 1 to 2 direct
  {RouteSegment(segments[0]),RouteSegment(segments[1]),RouteSegment(segments[3]),RouteSegment(segments[4],true)}, // 1 to 3 direct via 2
  {RouteSegment(segments[0]),RouteSegment(segments[7], true),RouteSegment(segments[5], true),RouteSegment(segments[4],true)}, // 1 to 3 direct via 4
  {RouteSegment(segments[0]),RouteSegment(segments[7],true),RouteSegment(segments[6],true)}, // 1 to 4 direct
  {RouteSegment(segments[2]),RouteSegment(segments[1],true),RouteSegment(segments[0],true)},// 2 to 1 direct
  {RouteSegment(segments[2]),RouteSegment(segments[3]),RouteSegment(segments[4],true)},// 2 to 3 direct
  {RouteSegment(segments[2]),RouteSegment(segments[1],true),RouteSegment(segments[7],true),RouteSegment(segments[6],true)},// 2 to 4 via 1
  {RouteSegment(segments[2]),RouteSegment(segments[3]),RouteSegment(segments[5]),RouteSegment(segments[6],true)},// 2 to 4 via 3
  {RouteSegment(segments[4]),RouteSegment(segments[3],true),RouteSegment(segments[1],true),RouteSegment(segments[0],true)},// 3 to 1 via 2
  {RouteSegment(segments[4]),RouteSegment(segments[5]),RouteSegment(segments[7]),RouteSegment(segments[0],true)},// 3 to 1 via 4
  {RouteSegment(segments[4]),RouteSegment(segments[3],true),RouteSegment(segments[2],true)},// 3 to 2 direct
  {RouteSegment(segments[4]),RouteSegment(segments[5]),RouteSegment(segments[6],true)},// 3 to 4 direct
  {RouteSegment(segments[6]),RouteSegment(segments[7]),RouteSegment(segments[0],true)},// 4 to 1 direct
  {RouteSegment(segments[6]),RouteSegment(segments[5],true),RouteSegment(segments[4],true)},// 4 to 3 direct
  {RouteSegment(segments[6]),RouteSegment(segments[7]),RouteSegment(segments[1]),RouteSegment(segments[2],true)},// 4 to 2 via 1
  {RouteSegment(segments[6]),RouteSegment(segments[5],true),RouteSegment(segments[3],true),RouteSegment(segments[2],true)}// 4 to 2 via 3
  };
Route routes[] = {
  Route(route_vector[0]),
  Route(route_vector[1]),
  Route(route_vector[2]),
  Route(route_vector[3]),
  Route(route_vector[4]),
  Route(route_vector[5]),
  Route(route_vector[6]),
  Route(route_vector[7]),
  Route(route_vector[8]),
  Route(route_vector[9]),
  Route(route_vector[10]),
  Route(route_vector[11]),
  Route(route_vector[12]),
  Route(route_vector[13]),
  Route(route_vector[14]),
  Route(route_vector[15])
};

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();


    for(Route r: routes) {
      Serial.println("Route");
      Serial.printf("Length: %d",r.length);
    }
    Serial.println();
    Serial.println("Running...");
}
void renderPackets() {
  for(Packet p: packets) {
    Route r = p.route;
    int p_count = 0;
    for(RouteSegment rs: r.segments) {
      Segment s = rs.segment;
      if(rs.reverse) {
        for(int i=(s.length-1);i>=0;i--) {
          if(p_count==p.position) {
            strip.SetPixelColor(s.offset+i,p.color);
          }
          for(int j=0;j<fade_length;j++) {
            if(p_count==p.position-j) {
              RgbColor dim = RgbColor::LinearBlend(p.color,s.background,(float)j/fade_length);
              strip.SetPixelColor(s.offset+i,dim);
            }
          }
          p_count++;
        }
      } else {
        for(int i=0;i<s.length;i++) {
          if(p_count==p.position) {
            strip.SetPixelColor(s.offset+i,p.color);
          }
          for(int j=0;j<fade_length;j++) {
            if(p_count==p.position-j) {
              RgbColor dim = RgbColor::LinearBlend(p.color,s.background,(float)j/fade_length);
              strip.SetPixelColor(s.offset+i,dim);
            }
          }
          p_count++;
        }
      }
    }
  }
}
uint8_t getRandom(uint8_t min, uint8_t max) {
  uint8_t r = esp_random()/24;
  return (r * (max - min)) / (UINT8_MAX + min);
}
void loop()
{
  packet_interval = map(analogRead(34),0,4095,500,20000);
  fade_length = map(analogRead(35),0,4095,1,10);
  if ((last_render+render_interval) < millis()) {
    last_render = millis();
    // Set background color for all segments
    for(Segment s: segments) {
      for(int i=0;i<s.length;i++) {
        strip.SetPixelColor(s.offset+i,s.background);
      }
    }
    // render packets on routes here
    renderPackets();
    strip.Show();
  }
  if ((last_packet_next+packet_next_interval) < millis()) {
    last_packet_next = millis();
    // Vector Magic necessary due to remove logic
    std::vector<Packet>::iterator it = packets.begin();
    while(it != packets.end()) {
      // Increase Position
      (*it).position+=1;
      // Kill packet when finished (full path+shadow)
      if((*it).position >= ((*it).route.length+fade_length)) {
        it = packets.erase(it);
      } else {
        ++it;
      }
    }
  }
  if ((last_packet+packet_interval) < millis()) {
    last_packet = millis();
    Serial.println("Spawn packet");
    // Choose Random Color
    uint8_t r = random(0,(sizeof(packetColor)/sizeof(RgbColor)));
    RgbColor p_col = packetColor[r];
    // Choose Random Route
    uint8_t r_ind = random(0,(sizeof(routes)/sizeof(Route)));
    Route route = routes[r_ind];
    packets.push_back(Packet(route,p_col));
  }
}