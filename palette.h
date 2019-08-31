#ifndef PALETTE_H
#define PALETTE_H

#include "color.h"
#include "range.h"

class Palette {
public:
  Palette() {}

  Palette(const Color& color1, const Color& color2) {
    addKey(Key(0.0, color1));
    addKey(Key(1.0, color2));
  }

  Palette(std::initializer_list<Color> colors) {
    double index = 0.0;
    double step = 1.0 / (colors.size() - 1);
    for (auto c = colors.begin(); c != colors.end(); ++c) {
      addKey(Key(index , *c));
      index += step;
    }
  }

  struct Key {
    Key(double val, const Color& color) : val(val), color(color) {}
    double val;
    Color color;
  };

  void addKey(const Key& key) {
    keys.push_back(key);
  }

  Color color(double val) const {
    for (int i = 0; i < keys.size(); ++i) {
      if (val < keys[i].val) {
        if (i == 0) {
          return keys[0].color;
        }
        const Key& a = keys[i - 1];
        const Key& b = keys[i];
        double alpha = interpolate(val, Range(a.val, b.val), Range());
        return interpolate_colors(alpha, a.color, b.color);
      }
    }
    return keys[keys.size() - 1].color;
  }

  static Palette Veridis() {
    Palette p;
    p.addKey(Key(0.0, Color(0, 32, 76)));
    p.addKey(Key(0.00392156862745098, Color(0, 32, 78)));
    p.addKey(Key(0.00784313725490196, Color(0, 33, 80)));
    p.addKey(Key(0.011764705882352941, Color(0, 34, 81)));
    p.addKey(Key(0.01568627450980392, Color(0, 35, 83)));
    p.addKey(Key(0.0196078431372549, Color(0, 35, 85)));
    p.addKey(Key(0.023529411764705882, Color(0, 36, 86)));
    p.addKey(Key(0.027450980392156862, Color(0, 37, 88)));
    p.addKey(Key(0.03137254901960784, Color(0, 38, 90)));
    p.addKey(Key(0.03529411764705882, Color(0, 38, 91)));
    p.addKey(Key(0.0392156862745098, Color(0, 39, 93)));
    p.addKey(Key(0.043137254901960784, Color(0, 40, 95)));
    p.addKey(Key(0.047058823529411764, Color(0, 40, 97)));
    p.addKey(Key(0.050980392156862744, Color(0, 41, 99)));
    p.addKey(Key(0.054901960784313725, Color(0, 42, 100)));
    p.addKey(Key(0.058823529411764705, Color(0, 42, 102)));
    p.addKey(Key(0.06274509803921569, Color(0, 43, 104)));
    p.addKey(Key(0.06666666666666667, Color(0, 44, 106)));
    p.addKey(Key(0.07058823529411765, Color(0, 45, 108)));
    p.addKey(Key(0.07450980392156863, Color(0, 45, 109)));
    p.addKey(Key(0.0784313725490196, Color(0, 46, 110)));
    p.addKey(Key(0.08235294117647059, Color(0, 46, 111)));
    p.addKey(Key(0.08627450980392157, Color(0, 47, 111)));
    p.addKey(Key(0.09019607843137255, Color(0, 47, 111)));
    p.addKey(Key(0.09411764705882353, Color(0, 48, 111)));
    p.addKey(Key(0.09803921568627451, Color(0, 49, 111)));
    p.addKey(Key(0.10196078431372549, Color(0, 49, 111)));
    p.addKey(Key(0.10588235294117647, Color(0, 50, 110)));
    p.addKey(Key(0.10980392156862745, Color(0, 51, 110)));
    p.addKey(Key(0.11372549019607843, Color(0, 52, 110)));
    p.addKey(Key(0.11764705882352941, Color(0, 52, 110)));
    p.addKey(Key(0.12156862745098039, Color(1, 53, 110)));
    p.addKey(Key(0.12549019607843137, Color(6, 54, 110)));
    p.addKey(Key(0.12941176470588237, Color(10, 55, 109)));
    p.addKey(Key(0.13333333333333333, Color(14, 55, 109)));
    p.addKey(Key(0.13725490196078433, Color(18, 56, 109)));
    p.addKey(Key(0.1411764705882353, Color(21, 57, 109)));
    p.addKey(Key(0.1450980392156863, Color(23, 57, 109)));
    p.addKey(Key(0.14901960784313725, Color(26, 58, 108)));
    p.addKey(Key(0.15294117647058825, Color(28, 59, 108)));
    p.addKey(Key(0.1568627450980392, Color(30, 60, 108)));
    p.addKey(Key(0.1607843137254902, Color(32, 60, 108)));
    p.addKey(Key(0.16470588235294117, Color(34, 61, 108)));
    p.addKey(Key(0.16862745098039217, Color(36, 62, 108)));
    p.addKey(Key(0.17254901960784313, Color(38, 62, 108)));
    p.addKey(Key(0.17647058823529413, Color(39, 63, 108)));
    p.addKey(Key(0.1803921568627451, Color(41, 64, 107)));
    p.addKey(Key(0.1843137254901961, Color(43, 65, 107)));
    p.addKey(Key(0.18823529411764706, Color(44, 65, 107)));
    p.addKey(Key(0.19215686274509805, Color(46, 66, 107)));
    p.addKey(Key(0.19607843137254902, Color(47, 67, 107)));
    p.addKey(Key(0.2, Color(49, 68, 107)));
    p.addKey(Key(0.20392156862745098, Color(50, 68, 107)));
    p.addKey(Key(0.20784313725490197, Color(51, 69, 107)));
    p.addKey(Key(0.21176470588235294, Color(53, 70, 107)));
    p.addKey(Key(0.21568627450980393, Color(54, 70, 107)));
    p.addKey(Key(0.2196078431372549, Color(55, 71, 107)));
    p.addKey(Key(0.2235294117647059, Color(56, 72, 107)));
    p.addKey(Key(0.22745098039215686, Color(58, 73, 107)));
    p.addKey(Key(0.23137254901960785, Color(59, 73, 107)));
    p.addKey(Key(0.23529411764705882, Color(60, 74, 107)));
    p.addKey(Key(0.23921568627450981, Color(61, 75, 107)));
    p.addKey(Key(0.24313725490196078, Color(62, 75, 107)));
    p.addKey(Key(0.24705882352941178, Color(64, 76, 107)));
    p.addKey(Key(0.25098039215686274, Color(65, 77, 107)));
    p.addKey(Key(0.2549019607843137, Color(66, 78, 107)));
    p.addKey(Key(0.25882352941176473, Color(67, 78, 107)));
    p.addKey(Key(0.2627450980392157, Color(68, 79, 107)));
    p.addKey(Key(0.26666666666666666, Color(69, 80, 107)));
    p.addKey(Key(0.27058823529411763, Color(70, 80, 107)));
    p.addKey(Key(0.27450980392156865, Color(71, 81, 107)));
    p.addKey(Key(0.2784313725490196, Color(72, 82, 107)));
    p.addKey(Key(0.2823529411764706, Color(73, 83, 107)));
    p.addKey(Key(0.28627450980392155, Color(74, 83, 107)));
    p.addKey(Key(0.2901960784313726, Color(75, 84, 107)));
    p.addKey(Key(0.29411764705882354, Color(76, 85, 107)));
    p.addKey(Key(0.2980392156862745, Color(77, 85, 107)));
    p.addKey(Key(0.30196078431372547, Color(78, 86, 107)));
    p.addKey(Key(0.3058823529411765, Color(79, 87, 108)));
    p.addKey(Key(0.30980392156862746, Color(80, 88, 108)));
    p.addKey(Key(0.3137254901960784, Color(81, 88, 108)));
    p.addKey(Key(0.3176470588235294, Color(82, 89, 108)));
    p.addKey(Key(0.3215686274509804, Color(83, 90, 108)));
    p.addKey(Key(0.3254901960784314, Color(84, 90, 108)));
    p.addKey(Key(0.32941176470588235, Color(85, 91, 108)));
    p.addKey(Key(0.3333333333333333, Color(86, 92, 108)));
    p.addKey(Key(0.33725490196078434, Color(87, 93, 109)));
    p.addKey(Key(0.3411764705882353, Color(88, 93, 109)));
    p.addKey(Key(0.34509803921568627, Color(89, 94, 109)));
    p.addKey(Key(0.34901960784313724, Color(90, 95, 109)));
    p.addKey(Key(0.35294117647058826, Color(91, 95, 109)));
    p.addKey(Key(0.3568627450980392, Color(92, 96, 109)));
    p.addKey(Key(0.3607843137254902, Color(93, 97, 110)));
    p.addKey(Key(0.36470588235294116, Color(94, 98, 110)));
    p.addKey(Key(0.3686274509803922, Color(95, 98, 110)));
    p.addKey(Key(0.37254901960784315, Color(95, 99, 110)));
    p.addKey(Key(0.3764705882352941, Color(96, 100, 110)));
    p.addKey(Key(0.3803921568627451, Color(97, 101, 111)));
    p.addKey(Key(0.3843137254901961, Color(98, 101, 111)));
    p.addKey(Key(0.38823529411764707, Color(99, 102, 111)));
    p.addKey(Key(0.39215686274509803, Color(100, 103, 111)));
    p.addKey(Key(0.396078431372549, Color(101, 103, 111)));
    p.addKey(Key(0.4, Color(102, 104, 112)));
    p.addKey(Key(0.403921568627451, Color(103, 105, 112)));
    p.addKey(Key(0.40784313725490196, Color(104, 106, 112)));
    p.addKey(Key(0.4117647058823529, Color(104, 106, 112)));
    p.addKey(Key(0.41568627450980394, Color(105, 107, 113)));
    p.addKey(Key(0.4196078431372549, Color(106, 108, 113)));
    p.addKey(Key(0.4235294117647059, Color(107, 109, 113)));
    p.addKey(Key(0.42745098039215684, Color(108, 109, 114)));
    p.addKey(Key(0.43137254901960786, Color(109, 110, 114)));
    p.addKey(Key(0.43529411764705883, Color(110, 111, 114)));
    p.addKey(Key(0.4392156862745098, Color(111, 111, 114)));
    p.addKey(Key(0.44313725490196076, Color(111, 112, 115)));
    p.addKey(Key(0.4470588235294118, Color(112, 113, 115)));
    p.addKey(Key(0.45098039215686275, Color(113, 114, 115)));
    p.addKey(Key(0.4549019607843137, Color(114, 114, 116)));
    p.addKey(Key(0.4588235294117647, Color(115, 115, 116)));
    p.addKey(Key(0.4627450980392157, Color(116, 116, 117)));
    p.addKey(Key(0.4666666666666667, Color(117, 117, 117)));
    p.addKey(Key(0.47058823529411764, Color(117, 117, 117)));
    p.addKey(Key(0.4745098039215686, Color(118, 118, 118)));
    p.addKey(Key(0.47843137254901963, Color(119, 119, 118)));
    p.addKey(Key(0.4823529411764706, Color(120, 120, 118)));
    p.addKey(Key(0.48627450980392156, Color(121, 120, 119)));
    p.addKey(Key(0.49019607843137253, Color(122, 121, 119)));
    p.addKey(Key(0.49411764705882355, Color(123, 122, 119)));
    p.addKey(Key(0.4980392156862745, Color(123, 123, 120)));
    p.addKey(Key(0.5019607843137255, Color(124, 123, 120)));
    p.addKey(Key(0.5058823529411764, Color(125, 124, 120)));
    p.addKey(Key(0.5098039215686274, Color(126, 125, 120)));
    p.addKey(Key(0.5137254901960784, Color(127, 126, 120)));
    p.addKey(Key(0.5176470588235295, Color(128, 126, 120)));
    p.addKey(Key(0.5215686274509804, Color(129, 127, 120)));
    p.addKey(Key(0.5254901960784314, Color(130, 128, 120)));
    p.addKey(Key(0.5294117647058824, Color(131, 129, 120)));
    p.addKey(Key(0.5333333333333333, Color(132, 129, 120)));
    p.addKey(Key(0.5372549019607843, Color(133, 130, 120)));
    p.addKey(Key(0.5411764705882353, Color(134, 131, 120)));
    p.addKey(Key(0.5450980392156862, Color(135, 132, 120)));
    p.addKey(Key(0.5490196078431373, Color(136, 133, 120)));
    p.addKey(Key(0.5529411764705883, Color(137, 133, 120)));
    p.addKey(Key(0.5568627450980392, Color(138, 134, 120)));
    p.addKey(Key(0.5607843137254902, Color(139, 135, 120)));
    p.addKey(Key(0.5647058823529412, Color(140, 136, 120)));
    p.addKey(Key(0.5686274509803921, Color(141, 136, 120)));
    p.addKey(Key(0.5725490196078431, Color(142, 137, 120)));
    p.addKey(Key(0.5764705882352941, Color(143, 138, 120)));
    p.addKey(Key(0.5803921568627451, Color(144, 139, 120)));
    p.addKey(Key(0.5843137254901961, Color(145, 140, 120)));
    p.addKey(Key(0.5882352941176471, Color(146, 140, 120)));
    p.addKey(Key(0.592156862745098, Color(147, 141, 120)));
    p.addKey(Key(0.596078431372549, Color(148, 142, 120)));
    p.addKey(Key(0.6, Color(149, 143, 120)));
    p.addKey(Key(0.6039215686274509, Color(150, 143, 119)));
    p.addKey(Key(0.6078431372549019, Color(151, 144, 119)));
    p.addKey(Key(0.611764705882353, Color(152, 145, 119)));
    p.addKey(Key(0.615686274509804, Color(153, 146, 119)));
    p.addKey(Key(0.6196078431372549, Color(154, 147, 119)));
    p.addKey(Key(0.6235294117647059, Color(155, 147, 119)));
    p.addKey(Key(0.6274509803921569, Color(156, 148, 119)));
    p.addKey(Key(0.6313725490196078, Color(157, 149, 119)));
    p.addKey(Key(0.6352941176470588, Color(158, 150, 118)));
    p.addKey(Key(0.6392156862745098, Color(159, 151, 118)));
    p.addKey(Key(0.6431372549019608, Color(160, 152, 118)));
    p.addKey(Key(0.6470588235294118, Color(161, 152, 118)));
    p.addKey(Key(0.6509803921568628, Color(162, 153, 118)));
    p.addKey(Key(0.6549019607843137, Color(163, 154, 117)));
    p.addKey(Key(0.6588235294117647, Color(164, 155, 117)));
    p.addKey(Key(0.6627450980392157, Color(165, 156, 117)));
    p.addKey(Key(0.6666666666666666, Color(166, 156, 117)));
    p.addKey(Key(0.6705882352941176, Color(167, 157, 117)));
    p.addKey(Key(0.6745098039215687, Color(168, 158, 116)));
    p.addKey(Key(0.6784313725490196, Color(169, 159, 116)));
    p.addKey(Key(0.6823529411764706, Color(170, 160, 116)));
    p.addKey(Key(0.6862745098039216, Color(171, 161, 116)));
    p.addKey(Key(0.6901960784313725, Color(172, 161, 115)));
    p.addKey(Key(0.6941176470588235, Color(173, 162, 115)));
    p.addKey(Key(0.6980392156862745, Color(174, 163, 115)));
    p.addKey(Key(0.7019607843137254, Color(175, 164, 115)));
    p.addKey(Key(0.7058823529411765, Color(176, 165, 114)));
    p.addKey(Key(0.7098039215686275, Color(177, 166, 114)));
    p.addKey(Key(0.7137254901960784, Color(178, 166, 114)));
    p.addKey(Key(0.7176470588235294, Color(180, 167, 113)));
    p.addKey(Key(0.7215686274509804, Color(181, 168, 113)));
    p.addKey(Key(0.7254901960784313, Color(182, 169, 113)));
    p.addKey(Key(0.7294117647058823, Color(183, 170, 112)));
    p.addKey(Key(0.7333333333333333, Color(184, 171, 112)));
    p.addKey(Key(0.7372549019607844, Color(185, 171, 112)));
    p.addKey(Key(0.7411764705882353, Color(186, 172, 111)));
    p.addKey(Key(0.7450980392156863, Color(187, 173, 111)));
    p.addKey(Key(0.7490196078431373, Color(188, 174, 110)));
    p.addKey(Key(0.7529411764705882, Color(189, 175, 110)));
    p.addKey(Key(0.7568627450980392, Color(190, 176, 110)));
    p.addKey(Key(0.7607843137254902, Color(191, 177, 109)));
    p.addKey(Key(0.7647058823529411, Color(192, 177, 109)));
    p.addKey(Key(0.7686274509803922, Color(193, 178, 108)));
    p.addKey(Key(0.7725490196078432, Color(194, 179, 108)));
    p.addKey(Key(0.7764705882352941, Color(195, 180, 108)));
    p.addKey(Key(0.7803921568627451, Color(197, 181, 107)));
    p.addKey(Key(0.7843137254901961, Color(198, 182, 107)));
    p.addKey(Key(0.788235294117647, Color(199, 183, 106)));
    p.addKey(Key(0.792156862745098, Color(200, 184, 106)));
    p.addKey(Key(0.796078431372549, Color(201, 184, 105)));
    p.addKey(Key(0.8, Color(202, 185, 105)));
    p.addKey(Key(0.803921568627451, Color(203, 186, 104)));
    p.addKey(Key(0.807843137254902, Color(204, 187, 104)));
    p.addKey(Key(0.8117647058823529, Color(205, 188, 103)));
    p.addKey(Key(0.8156862745098039, Color(206, 189, 103)));
    p.addKey(Key(0.8196078431372549, Color(208, 190, 102)));
    p.addKey(Key(0.8235294117647058, Color(209, 191, 102)));
    p.addKey(Key(0.8274509803921568, Color(210, 192, 101)));
    p.addKey(Key(0.8313725490196079, Color(211, 192, 101)));
    p.addKey(Key(0.8352941176470589, Color(212, 193, 100)));
    p.addKey(Key(0.8392156862745098, Color(213, 194, 99)));
    p.addKey(Key(0.8431372549019608, Color(214, 195, 99)));
    p.addKey(Key(0.8470588235294118, Color(215, 196, 98)));
    p.addKey(Key(0.8509803921568627, Color(216, 197, 97)));
    p.addKey(Key(0.8549019607843137, Color(217, 198, 97)));
    p.addKey(Key(0.8588235294117647, Color(219, 199, 96)));
    p.addKey(Key(0.8627450980392157, Color(220, 200, 96)));
    p.addKey(Key(0.8666666666666667, Color(221, 201, 95)));
    p.addKey(Key(0.8705882352941177, Color(222, 202, 94)));
    p.addKey(Key(0.8745098039215686, Color(223, 203, 93)));
    p.addKey(Key(0.8784313725490196, Color(224, 203, 93)));
    p.addKey(Key(0.8823529411764706, Color(225, 204, 92)));
    p.addKey(Key(0.8862745098039215, Color(227, 205, 91)));
    p.addKey(Key(0.8901960784313725, Color(228, 206, 91)));
    p.addKey(Key(0.8941176470588236, Color(229, 207, 90)));
    p.addKey(Key(0.8980392156862745, Color(230, 208, 89)));
    p.addKey(Key(0.9019607843137255, Color(231, 209, 88)));
    p.addKey(Key(0.9058823529411765, Color(232, 210, 87)));
    p.addKey(Key(0.9098039215686274, Color(233, 211, 86)));
    p.addKey(Key(0.9137254901960784, Color(235, 212, 86)));
    p.addKey(Key(0.9176470588235294, Color(236, 213, 85)));
    p.addKey(Key(0.9215686274509803, Color(237, 214, 84)));
    p.addKey(Key(0.9254901960784314, Color(238, 215, 83)));
    p.addKey(Key(0.9294117647058824, Color(239, 216, 82)));
    p.addKey(Key(0.9333333333333333, Color(240, 217, 81)));
    p.addKey(Key(0.9372549019607843, Color(241, 218, 80)));
    p.addKey(Key(0.9411764705882353, Color(243, 219, 79)));
    p.addKey(Key(0.9450980392156862, Color(244, 220, 78)));
    p.addKey(Key(0.9490196078431372, Color(245, 221, 77)));
    p.addKey(Key(0.9529411764705882, Color(246, 222, 76)));
    p.addKey(Key(0.9568627450980393, Color(247, 223, 75)));
    p.addKey(Key(0.9607843137254902, Color(249, 224, 73)));
    p.addKey(Key(0.9647058823529412, Color(250, 224, 72)));
    p.addKey(Key(0.9686274509803922, Color(251, 225, 71)));
    p.addKey(Key(0.9725490196078431, Color(252, 226, 70)));
    p.addKey(Key(0.9764705882352941, Color(253, 227, 69)));
    p.addKey(Key(0.9803921568627451, Color(255, 228, 67)));
    p.addKey(Key(0.984313725490196, Color(255, 229, 66)));
    p.addKey(Key(0.9882352941176471, Color(255, 230, 66)));
    p.addKey(Key(0.9921568627450981, Color(255, 231, 67)));
    p.addKey(Key(0.996078431372549, Color(255, 232, 68)));
    p.addKey(Key(1.0, Color(255, 233, 69)));
    return p;
  }

private:
  std::vector<Key> keys;
};

#endif
