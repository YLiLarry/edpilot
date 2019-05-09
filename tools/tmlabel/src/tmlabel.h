#include <ggframe.h>
#include <vector>

#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>

namespace tmlabel
{

    using namespace std;
    using namespace ggframe;
    using ggframe::Size;
    using namespace cv;
    using namespace cv::xfeatures2d;

    auto feature_algorithm = cv::xfeatures2d::SIFT::create(0, 3, 0.08, 10, 0.4);

    class TemplateMatcher
    {
    private:
        float m_ratio_test = 0.85;
    protected:
        Frame m_template;
        Frame m_target;
        vector<cv::DMatch> m_matches;
    public:
        TemplateMatcher() = default;
        TemplateMatcher(TemplateMatcher const& other);
        TemplateMatcher(Frame const& pattern);
        void setTemplateFrame(Frame const& pattern);
        void setTargetFrame(Frame const& target);
        Frame& templateFrame();
        Frame const& templateFrame() const;
        void computeTemplateKeypoints();
        void computeMatches();
        unsigned countMatchesInRec(Rec const& rec);
        void drawMatches(Frame& canvas) const;
    };

    class TemplateMatchingLabeler
    {
    private:
        unsigned m_grid_size = 10;
        vector<TemplateMatcher> m_template_matchers;
    public:
        TemplateMatchingLabeler() = default;
        void addTemplateMatcher(TemplateMatcher const& matcher);
        vector<TemplateMatcher>& templateMatchers();
        vector<TemplateMatcher> const& templateMatchers() const;
        void setGridSize(unsigned size);
        Rec bestRecMatchForFrame(Frame& frame, Size const& size);
    };

}
