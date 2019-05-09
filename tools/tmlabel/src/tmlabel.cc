#include <string>
#include <iostream>
#include <filesystem>
#include <tmlabel.h>

#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace ggframe;
using namespace tmlabel;
using namespace cv;
using namespace cv::xfeatures2d;
using ggframe::Size;

int main() {
    TemplateMatchingLabeler labeler;

    filesystem::path data_dir;
    /* define where data is located */
#if WIN32
    data_dir = "C:\\Users\\Yu\\Desktop\\data";
#endif
#if APPLE
    data_dir = "/Users/yuli/Downloads/data";
#endif
    filesystem::path template_dir = data_dir / "templates";
    std::cerr << "checking " << template_dir << std::endl;
    for (auto& file : filesystem::directory_iterator(template_dir))
    {
        string fp = file.path().string();
        /* found a template image */
        if (fp.find(".bmp") != fp.npos) {
            std::cerr << "found template " << fp << std::endl;
            Frame pattern(file.path());
            TemplateMatcher matcher(pattern);
            matcher.computeTemplateKeypoints();
            labeler.addTemplateMatcher(matcher);
        }
    }
    std::cerr << "checking " << data_dir << std::endl;
    for (auto& file : filesystem::directory_iterator(data_dir))
    {
        string fp = file.path().string();
        /* found an unlabeld image */
        if (fp.find(".bmp") != fp.npos) {
            std::cerr << fp << " ";
            Frame frame(fp);
            frame.computeKeypoints(tmlabel::feature_algorithm);
            Rec rec = labeler.bestRecMatchForFrame(frame, ggframe::Size::hw(200, 150));
            cerr << rec << endl;
            TemplateMatcher const& matcher = labeler.templateMatchers()[0];
            matcher.drawMatches(frame);
            int offset = matcher.templateFrame().nCols();
            Rec offset_rec = Rec::tlbr(rec.top(), rec.left() + offset, rec.bottom(), rec.right() + offset);
            frame.drawRec(offset_rec);
            frame.display();
        }
    }
}

void TemplateMatchingLabeler::addTemplateMatcher(TemplateMatcher const& matcher)
{
    m_template_matchers.push_back(matcher);
}

void TemplateMatcher::computeTemplateKeypoints()
{
    m_template.computeKeypoints(tmlabel::feature_algorithm);
}

void TemplateMatcher::computeMatches()
{
    assert(m_target.hasKeypoints() && "did you precompute keypoints?");
    assert(m_template.hasKeypoints() && "did you precompute keypoints?");
    m_matches.clear();
    /* filter matches */
    auto bforce = cv::BFMatcher::create(NORM_L2, false);
    vector<vector<cv::DMatch>> matches;
    bforce->add(m_target.descriptors());
    bforce->knnMatch(m_template.descriptors(), m_target.descriptors(), matches, 2);
    /* ratio test */
    for (vector<cv::DMatch> const& m : matches) {
        cv::DMatch const& fst_match = m[0];
        cv::DMatch const& snd_match = m[1];
        vector<char> local_mask;
        if (fst_match.distance / snd_match.distance < m_ratio_test) {
            m_matches.push_back(fst_match);
        }
    }
}

void TemplateMatcher::drawMatches(Frame& canvas) const
{
    cv::drawMatches(m_template.cvMat(), m_template.keypoints(), m_target.cvMat(), m_target.keypoints(), m_matches, canvas.cvMat());
}

Rec TemplateMatchingLabeler::bestRecMatchForFrame(Frame& frame, ggframe::Size const& size)
{
    Rec current_best = Rec::tlbr(0, 0, size.height() - 1, size.width() - 1);
    int current_best_sum = 0;
    for (auto& m : m_template_matchers) {
        m.setTargetFrame(frame);
        m.computeMatches();
    }
    for (int r = 0; r < frame.nRows(); r += m_grid_size) {
        for (int c = 0; c < frame.nCols(); c += m_grid_size) {
            Rec next = Rec::tlbr(r, c, r + size.height() - 1, c + size.width() - 1);
            int sum = 0;
            for (auto& m : m_template_matchers) {
                sum += m.countMatchesInRec(next);
            }
            if (sum > current_best_sum) {
                current_best = next;
                current_best_sum = sum;
            }
        }
    }
    return current_best;
}

void TemplateMatcher::setTargetFrame(Frame const& target)
{
    m_target = target;
}

unsigned TemplateMatcher::countMatchesInRec(Rec const& rec)
{
    assert(m_matches.size() && "did you compute matches?");
    unsigned good_count = 0;
    /* consider using kd-tree */
    for (cv::DMatch const& m : m_matches) {
        cv::KeyPoint const& kp = m_target.keypoints()[m.trainIdx];
        if (rec.containsPos(Pos::rc(kp.pt.y, kp.pt.x))) {
            good_count++;
        }
    }
    return good_count;
}

TemplateMatcher::TemplateMatcher(Frame const& pattern)
{
    m_template = pattern;
}

TemplateMatcher::TemplateMatcher(TemplateMatcher const& other)
{
    m_template = other.m_template;
    m_target = other.m_target;
}

vector<TemplateMatcher>& TemplateMatchingLabeler::templateMatchers() { return m_template_matchers; }
vector<TemplateMatcher> const& TemplateMatchingLabeler::templateMatchers() const { return m_template_matchers; }

Frame& TemplateMatcher::templateFrame() { return m_template; }
Frame const& TemplateMatcher::templateFrame() const { return m_template; }
