#include <string>
#include <iostream>
#include <tmlabel.h>
#include <fstream>

#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc.hpp>

#if APPLE
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif

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
    filesystem::path labeled_dir = data_dir / "labeled";
    filesystem::path label_files_dir = data_dir / "labels";
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
        string fname = file.path().filename().string();
        filesystem::path labeled_img_path = labeled_dir / ("labeled_" + fname);
        filesystem::path label_path = label_files_dir / ("label_" + fname.replace(fname.end()-3, fname.end(), "txt"));
        if (filesystem::exists(labeled_img_path)) {
            cerr << "skipping " << labeled_img_path << endl;
            continue;
        }
        /* found an unlabeld image */
        if (fp.find(".bmp") != fp.npos) {
            std::cerr << fp << " ";
            Frame frame(fp);
            frame.computeKeypoints(tmlabel::feature_algorithm);
            Rec best_rec = labeler.bestRecMatchForFrame(frame, ggframe::Size::hw(150, 100));
            cerr << best_rec << endl;
            TemplateMatcher const& matcher = labeler.templateMatchers()[1];
            matcher.drawMatches(frame);
            int offset = matcher.templateFrame().nCols();
            Rec offset_rec = Rec::tlbr(best_rec.top(), best_rec.left() + offset, best_rec.bottom(), best_rec.right() + offset);
            frame.drawRec(offset_rec);
            frame.display();
            frame.save(labeled_img_path);
            ofstream label_file_stream(label_path, ios_base::out|ios_base::trunc);
            label_file_stream << "tlbr "
                << best_rec.top() << " "
                << best_rec.left() << " "
                << best_rec.bottom() << " "
                << best_rec.right() << endl;
            label_file_stream.close();
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
    float current_best_score = 0;
    for (auto& m : m_template_matchers) {
        m.setTargetFrame(frame);
        m.computeMatches();
    }
    for (int r = 0; r <= frame.nRows() - size.height(); r += m_grid_size) {
        for (int c = 0; c <= frame.nCols() - size.width(); c += m_grid_size) {
            Rec next = Rec::tlbr(r, c, r + size.height() - 1, c + size.width() - 1);
            float sum = 0;
            for (auto& m : m_template_matchers) {
                sum += m.countMatchesInRec(next);
            }
            if (sum > current_best_score) {
                current_best = next;
                current_best_score = sum;
            }
            //Frame tmp = frame;
            //tmp.drawRec(next);
            //tmp.display();
            //cerr << current_best_score << endl;
        }
    }
    return current_best;
}

void TemplateMatcher::setTargetFrame(Frame const& target)
{
    m_target = target;
}

float TemplateMatcher::countMatchesInRec(Rec const& rec)
{
    assert(m_matches.size() && "did you compute matches?");
    unsigned good_count = 0;
    float dist_score = 0;
    /* find how many matches are there in the rectangle */
    /* consider using kd-tree */
    Pos center = rec.center();
    for (cv::DMatch const& m : m_matches) {
        cv::KeyPoint const& kp = m_target.keypoints()[m.trainIdx];
        Pos kp_pos = Pos::rc(kp.pt.y, kp.pt.x);
        if (rec.containsPos(kp_pos)) {
            good_count++;
            /* kp == center ==> dist_score += 1,
               kp == left ==> dist_score += 0 */
            dist_score += 1 - static_cast<float>(std::abs(kp_pos.col() - center.col()) * 2) / static_cast<float>(rec.width());
            dist_score += 1 - static_cast<float>(std::abs(kp_pos.row() - center.row()) * 2) / static_cast<float>(rec.height());
        }
    }
    dist_score /= static_cast<float>(2 * good_count);
    /* compute an normialized distance to center for all matches */
    return good_count + dist_score;
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
