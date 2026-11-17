#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <exception>
#include <limits>

using namespace std;

class UPSCException : public exception {
protected:
    string errorMessage;
public:
    UPSCException(const string& msg) : errorMessage(msg) {}
    const char* what() const noexcept override { return errorMessage.c_str(); }
};

class InvalidOptionException : public UPSCException {
public:
    InvalidOptionException(const string& msg) : UPSCException("[InvalidOptionException]: " + msg) {}
};

class MenuSelectionException : public UPSCException {
public:
    MenuSelectionException(const string& msg) : UPSCException("[MenuSelectionException]: " + msg) {}
};

class IEvaluable {
public:
    virtual ~IEvaluable() {}
    virtual double evaluateResponse(int chosenOption) const = 0;
    virtual double getMaxMarks() const = 0;
    virtual string getStageName() const = 0;rfew
};

class Scorecard;

class Candidate {
private:
    string name;
    int testsCompleted;
public:
    Candidate(const string& candidateName = "Aspirant") : name(candidateName), testsCompleted(0) {}
    ~Candidate() {}
    Candidate& setName(const string& name) { this->name = name; return *this; }
    Candidate& incrementTests() { this->testsCompleted++; return *this; }
    string getName() const { return this->name; }
    int getTestsCompleted() const { return this->testsCompleted; }
    friend void displayAuditSummary(const Candidate& cand, const Scorecard& card);
};

class Question : public IEvaluable {
public:
    class PeerMetrics {
    private:
        int totalAttempts;
        int correctAttempts;
    public:
        PeerMetrics(int total = 1000, int correct = 650) : totalAttempts(total), correctAttempts(correct) {}
        void recordAttempt(bool isCorrect) { totalAttempts++; if (isCorrect) correctAttempts++; }
        int getTotal() const { return totalAttempts; }
        int getCorrect() const { return correctAttempts; }
    };

protected:
    int id;
    int year;
    string subject;
    string prompt;
    vector<string> options;
    int correctOption;
    string explanation;
    PeerMetrics peerStats;

private:
    static int totalQuestionsLoaded;

public:
    Question(int qId, int pyqYear, const string& subj, const string& qPrompt, const vector<string>& opts, int corrOpt, const string& exp, int peerTotal = 1200, int peerCorrect = 780)
        : id(qId), year(pyqYear), subject(subj), prompt(qPrompt), options(opts), correctOption(corrOpt), explanation(exp), peerStats(peerTotal, peerCorrect) { totalQuestionsLoaded++; }
    virtual ~Question() {}
    static int getTotalQuestionsLoaded() { return totalQuestionsLoaded; }

    virtual double getPeerAccuracy() const final {
        if (peerStats.getTotal() == 0) return 0.0;
        return (static_cast<double>(peerStats.getCorrect()) / static_cast<double>(peerStats.getTotal())) * 100.0;
    }

    void updatePeerStats(bool isCorrect) { this->peerStats.recordAttempt(isCorrect); }
    int getId() const { return id; }
    int getYear() const { return year; }
    string getSubject() const { return subject; }
    int getCorrectOption() const { return correctOption; }
    string getExplanation() const { return explanation; }

    string getOptionText(int index) const {
        if (index >= 1 && index <= static_cast<int>(options.size())) return options[index - 1];
        return "Skipped / None";
    }

    char getOptionLetter(int optNum) const {
        if (optNum >= 1 && optNum <= 4) return static_cast<char>('A' + (optNum - 1));
        return '-';
    }

    virtual void printQuestionBanner() const {
        cout << "\n--------------------------------------------------------------------\n";
        cout << "[" << getStageName() << " | " << subject << " | UPSC PYQ " << year << "]\n";
    }

    friend ostream& operator<<(ostream& os, const Question& q) {
        q.printQuestionBanner();
        os << "Q. " << q.prompt << "\n\n";
        for (size_t i = 0; i < q.options.size(); ++i) {
            char label = static_cast<char>('A' + i);
            os << "  [" << (i + 1) << "] (" << label << ") " << q.options[i] << "\n";
        }
        os << "  [0] Skip Question\n--------------------------------------------------------------------\n";
        return os;
    }
};

int Question::totalQuestionsLoaded = 0;

class PrelimsQuestion : public Question {
private:
    double positiveMarks;
    double negativeMarks;
public:
    PrelimsQuestion(int qId, int pyqYear, const string& subj, const string& qPrompt, const vector<string>& opts, int corrOpt, const string& exp, int peerTotal = 1400, int peerCorrect = 910)
        : Question(qId, pyqYear, subj, qPrompt, opts, corrOpt, exp, peerTotal, peerCorrect), positiveMarks(2.00), negativeMarks(0.66) {}
    ~PrelimsQuestion() override {}

    double evaluateResponse(int chosenOption) const override {
        if (chosenOption < 0 || chosenOption > 4) throw InvalidOptionException("Please enter an option between 1 and 4 (or 0 to skip).");
        if (chosenOption == 0) return 0.0;
        return (chosenOption == correctOption) ? positiveMarks : -negativeMarks;
    }

    double getMaxMarks() const override { return positiveMarks; }
    string getStageName() const override { return "UPSC PRELIMS"; }
    double getNegativePenalty() const { return negativeMarks; }
};

class MainsQuestion : public Question {
private:
    double maxMarks;
    int wordLimit;
public:
    MainsQuestion(int qId, int pyqYear, const string& subj, const string& qPrompt, const vector<string>& opts, int corrOpt, const string& exp, int wl = 150, int peerTotal = 950, int peerCorrect = 610)
        : Question(qId, pyqYear, subj, qPrompt, opts, corrOpt, exp, peerTotal, peerCorrect), maxMarks(10.00), wordLimit(wl) {}
    ~MainsQuestion() override {}

    double evaluateResponse(int chosenOption) const override {
        if (chosenOption < 0 || chosenOption > 4) throw InvalidOptionException("Please enter an option between 1 and 4 (or 0 to skip).");
        if (chosenOption == 0) return 0.0;
        return (chosenOption == correctOption) ? maxMarks : 0.0;
    }

    double getMaxMarks() const override { return maxMarks; }
    string getStageName() const override { return "UPSC MAINS"; }

    void printQuestionBanner() const override {
        cout << "\n====================================================================\n";
        cout << "[" << getStageName() << " | " << subject << " | PYQ " << year << " | " << static_cast<int>(maxMarks) << " Marks | " << wordLimit << " Words]\n";
        cout << "(Select the most accurate analytical thesis / framework option below)\n";
    }
};

class Scorecard final {
private:
    string sectionTitle;
    int totalQuestions, attempted, correct, incorrect, skipped;
    double scoreObtained, maxPossibleScore;
public:
    Scorecard(const string& title, int totalQ)
        : sectionTitle(title), totalQuestions(totalQ), attempted(0), correct(0), incorrect(0), skipped(0), scoreObtained(0.0), maxPossibleScore(0.0) {}

    void recordQuestionResult(int choice, int correctOpt, double pointsAwarded, double maxPts) {
        maxPossibleScore += maxPts;
        scoreObtained += pointsAwarded;
        if (choice == 0) skipped++;
        else { attempted++; if (choice == correctOpt) correct++; else incorrect++; }
    }

    friend void displayAuditSummary(const Candidate& cand, const Scorecard& card);
};

void displayAuditSummary(const Candidate& cand, const Scorecard& card) {
    double accuracy = (card.attempted > 0) ? (static_cast<double>(card.correct) / static_cast<double>(card.attempted)) * 100.0 : 0.0;
    cout << fixed << setprecision(2);
    cout << "\n********************************************************************\n";
    cout << "                   OFFICIAL UPSC MOCK TEST SCORECARD                \n";
    cout << "********************************************************************\n";
    cout << " Candidate Name      : " << cand.name << "\n";
    cout << " Paper / Subject     : " << card.sectionTitle << "\n";
    cout << " Total Questions     : " << card.totalQuestions << "\n";
    cout << " Attempted           : " << card.attempted << "\n";
    cout << " Correct Answers     : " << card.correct << "\n";
    cout << " Incorrect Answers   : " << card.incorrect << "\n";
    cout << " Skipped Questions   : " << card.skipped << "\n";
    cout << "--------------------------------------------------------------------\n";
    cout << " NET SCORE OBTAINED  : " << card.scoreObtained << " / " << card.maxPossibleScore << "\n";
    cout << " ACCURACY RATE       : " << accuracy << "%\n";
    cout << " Total Tests Taken   : " << cand.testsCompleted << "\n";
    cout << "********************************************************************\n";
}

class UPSCExamSystem {
private:
    vector<Question*> questionBank;

    void initializePYQBank() {
        questionBank.push_back(new PrelimsQuestion(1, 2023, "Polity", "Which one of the following statements best reflects the Chief Purpose of the 'Constitution' of a country?", {"It determines the objective for the making of necessary laws", "It enables the creation of political offices and a government", "It defines and limits the powers of government", "It secures social justice, social equality and social security"}, 3, "Constitutionalism fundamentally means limited government; a Constitution defines and restricts arbitrary state power (UPSC Prelims 2023).", 1850, 1420));
        questionBank.push_back(new PrelimsQuestion(2, 2021, "Polity", "Under the Indian Constitution, concentration of wealth violates:", {"The Right to Equality", "The Directive Principles of State Policy", "The Right to Freedom", "The Concept of Welfare"}, 2, "Article 39(c) under Part IV (DPSP) directs the State to ensure that the economic system does not result in the concentration of wealth (UPSC Prelims 2021).", 1620, 1290));
        questionBank.push_back(new PrelimsQuestion(3, 2022, "Polity", "If a particular area is brought under the Fifth Schedule of the Constitution of India, which one of the following statements best reflects its consequence?", {"This would prevent the transfer of land of tribal people to non-tribal people", "This would create a local self-governing body in that area", "This would convert that area into a Union Territory", "The State having such areas would be declared a Special Category State"}, 1, "The Fifth Schedule empowers the Governor to prohibit or restrict the transfer of land by or among members of Scheduled Tribes (UPSC Prelims 2022).", 1540, 1110));
        questionBank.push_back(new PrelimsQuestion(4, 2022, "Economy", "In India, which one of the following is responsible for maintaining price stability by controlling inflation?", {"Department of Consumer Affairs", "Expenditure Management Commission", "Financial Stability and Development Council", "Reserve Bank of India"}, 4, "Under the RBI Act, 1934 (amended in 2016), the primary objective of monetary policy by the RBI is maintaining price stability (UPSC Prelims 2022).", 1900, 1650));
        questionBank.push_back(new PrelimsQuestion(5, 2021, "Economy", "Which one of the following effects of creation of black money in India has been the main cause of worry to the Government of India?", {"Diversion of resources to the purchase of real estate and investment in luxury housing", "Investment in unproductive activities and purchase of precious stones, jewellery, gold, etc.", "Large donations to political parties and growth of regionalism", "Loss of revenue to the State Exchequer due to tax evasion"}, 4, "Tax evasion directly causes severe loss of revenue to the State Exchequer, undermining fiscal capacity (UPSC Prelims 2021).", 1480, 1020));
        questionBank.push_back(new PrelimsQuestion(6, 2020, "Economy", "If the RBI decides to adopt an expansionist monetary policy, which of the following would it NOT do?", {"Cut and optimize the Statutory Liquidity Ratio", "Increase the Marginal Standing Facility Rate", "Cut the Bank Rate and Repo Rate", "Engage in Open Market Operations to buy government securities"}, 2, "Increasing the MSF rate makes borrowing costlier (contractionary policy), whereas an expansionist policy lowers rates (UPSC Prelims 2020).", 1710, 1340));
        questionBank.push_back(new PrelimsQuestion(7, 2023, "Environment & History", "Which one of the following is the best example of repeated falls in sea level, giving rise to present-day extensive marshland?", {"Bhitarkanika Mangroves", "Marakkanam Salt Pans", "Naupada Swamp", "Rann of Kutch"}, 4, "The Rann of Kutch was once a shallow arm of the Arabian Sea; geological uplift and sea-level regression turned it into vast saline marshland (UPSC Prelims 2023).", 1390, 890));
        questionBank.push_back(new PrelimsQuestion(8, 2021, "Environment & History", "In the context of hereditary estate and social history, the 'Mirasi' system in South India was fundamentally associated with:", {"Hereditary village revenue and landholding rights under Ryotwari regions", "Military fiefdoms granted exclusively by the Vijayanagara Emperors", "Maritime guild taxation across Coromandel ports", "Temple architecture patronage committees"}, 1, "Mirasidars held hereditary land and village share rights in the Madras Presidency region (UPSC PYQ Theme 2020-2022).", 1120, 640));
        questionBank.push_back(new PrelimsQuestion(9, 2024, "Environment & History", "Which of the following organisms perform a waggle dance for others of their kin to indicate the direction and the distance to a source of their food?", {"Butterflies", "Dragonflies", "Honeybees", "Wasps"}, 3, "Honeybees perform the figure-eight 'waggle dance' to communicate the angle and distance of nectar/pollen sources (UPSC Prelims 2023).", 1750, 1480));
        questionBank.push_back(new PrelimsQuestion(10, 2023, "CSAT", "What is the remainder when (85 x 87 x 89 x 91 x 95 x 96) is divided by 100?", {"0", "1", "2", "4"}, 1, "85 (5x17), 95 (5x19), and 96 (4x24) contain factors 5 x 5 x 4 = 100. Since the product is a multiple of 100, the remainder is 0 (UPSC CSAT 2023).", 1600, 1280));
        questionBank.push_back(new PrelimsQuestion(11, 2022, "CSAT", "An identity card has the number ABCDEFG, not necessarily in that order, where each letter represents a distinct digit from {1, 2, 4, 5, 7, 8, 9}. The number is divisible by 9. After deleting the first digit from the right, the resulting number is divisible by 6. What is the sum of the digits of the number?", {"36", "35", "32", "28"}, 1, "Sum of {1 + 2 + 4 + 5 + 7 + 8 + 9} = 36, which is divisible by 9 (UPSC CSAT 2022).", 1320, 950));
        questionBank.push_back(new PrelimsQuestion(12, 2021, "CSAT", "In a group of 120 persons, 80 are Indians and rest are foreigners. Further, 70 persons in the group can speak English. The number of Indians who can speak English is:", {"20", "30", "30 or less", "30 or more"}, 4, "Foreigners = 120 - 80 = 40. Even if all 40 foreigners speak English, at least 70 - 40 = 30 Indians must speak English (UPSC CSAT 2021).", 1490, 1100));
        questionBank.push_back(new MainsQuestion(13, 2023, "GS-I (History & Society)", "Explain the role of geographical factors towards the development of Ancient India. (UPSC Mains GS-I 2023)", {"Himalayan barrier for climate/defense, fertile Indo-Gangetic alluvial plains supporting agrarian empires, and long coastline enabling Indo-Roman and Southeast Asian trade", "Complete isolation from Central Asia preventing any cultural or metallurgical exchange until the 18th century", "Absence of perennial river systems forcing ancient kingdoms to rely solely on arid pastoralism", "Uniform desert topography across the subcontinent preventing regional architectural diversity"}, 1, "Core Mains Rubric: 1. Himalayas as climatic/strategic shield & Khyber/Bolan passes; 2. Alluvial plains of Indus-Ganga nurturing Magadha/Mauryas; 3. Peninsular coast fostering maritime trade.", 150, 980, 740));
        questionBank.push_back(new MainsQuestion(14, 2022, "GS-I (History & Society)", "How will you explain that medieval Indian temple sculptures represent the social life of those days? (UPSC Mains GS-I 2022)", {"They depicted exclusively abstract geometric patterns devoid of human or societal imagery", "Carvings at Khajuraho, Konark, and Hampi vividly portray agrarian routines, musical instruments, martial processions, attire, and courtly life", "Medieval temples prohibited any carving outside strictly Vedic fire-altar diagrams", "Sculptures were imported entirely from Europe and reflected Renaissance guild structures"}, 2, "Core Mains Rubric: Temples functioned as social nuclei; friezes at Konark, Khajuraho, and Chola temples mirror contemporary dress, dance, warfare, economy, and customs.", 150, 910, 680));
        questionBank.push_back(new MainsQuestion(15, 2021, "GS-I (History & Society)", "Examine the uniqueness of tribal knowledge systems when compared with mainstream knowledge and cultural systems. (UPSC Mains GS-I 2021)", {"Tribal knowledge is exclusively codified in commercial patent registries and industrial manuals", "Oral transmission, ecological symbiosis, community ownership, ethno-botanical medicine, and sustainable resource stewardship", "Complete reliance on synthetic chemical inputs and monoculture forestry", "Rejection of biodiversity conservation in favor of urban industrialization"}, 2, "Core Mains Rubric: Holistic nature-culture continuum, oral intergenerational heritage, sacred groves, indigenous seed varieties, and collective custodian ethos.", 150, 890, 640));
        questionBank.push_back(new MainsQuestion(16, 2023, "GS-II (Polity & IR)", "Discuss the role of Presiding Officers of state legislatures in maintaining order and impartiality in conducting legislative work and facilitating best democratic practices. (UPSC Mains GS-II 2023)", {"Acting as an active partisan whip for the ruling coalition during floor tests", "Neutral arbitration of debates, safeguarding minority opposition voice, fair Tenth Schedule anti-defection adjudication (Kihoto Hollohan), and committee oversight", "Exercising veto power over Money Bills passed by the Rajya Sabha", "Directly appointing High Court judges during legislative recesses"}, 2, "Core Mains Rubric: Constitutional neutrality of the Speaker, Kihoto Hollohan (1992) judicial review over Tenth Schedule, protecting parliamentary deliberation.", 150, 940, 710));
        questionBank.push_back(new MainsQuestion(17, 2022, "GS-II (Polity & IR)", "Right of movement and residence throughout the territory of India are freely available to the Indian citizens, but these rights are not absolute. Comment. (UPSC Mains GS-II 2022)", {"Article 19(1)(d) and 19(1)(e) cannot be restricted under any circumstance, even during pandemics or in Scheduled Tribe areas", "Subject to reasonable restrictions under Article 19(5) in the interests of the general public and for the protection of the interests of any Scheduled Tribe (Inner Line Permit, Fifth/Sixth Schedule)", "Citizens require a federal passport to travel between any two Indian states", "Only Supreme Court judges enjoy the right to reside across state boundaries"}, 2, "Core Mains Rubric: Article 19(1)(d)/(e) read with Article 19(5) reasonable restrictions: public health, security, and protecting indigenous cultural/land integrity.", 150, 960, 730));
        questionBank.push_back(new MainsQuestion(18, 2021, "GS-II (Polity & IR)", "'If the last few decades were of Asia's growth story, the next few are expected to be of Africa's.' In the light of this statement, examine India's influence in Africa in recent years. (UPSC Mains GS-II 2021)", {"Demand-driven development partnership (Kampala Principles), Lines of Credit, Pan-African e-Network, solar alliance (ISA), and championing AU's permanent membership in G20", "Constructing exclusive military bases across West Africa while halting all bilateral trade", "Withdrawing from all diplomatic missions in Sub-Saharan Africa since 2015", "Imposing unilateral trade embargoes on the African Continental Free Trade Area (AfCFTA)"}, 1, "Core Mains Rubric: Ten Guiding Principles (Kampala 2018), capacity building (ITEC), vaccine maitri, maritime security in the Western Indian Ocean, and Global South solidarity.", 150, 870, 620));
        questionBank.push_back(new MainsQuestion(19, 2023, "GS-III (Economy & Security)", "Faster economic growth requires faster growth of the manufacturing sector. Give reasons and illustrate how unaddressed bottlenecks are being tackled by current policies. (UPSC Mains GS-III 2023)", {"Manufacturing has zero employment elasticity compared to subsistence farming", "High forward-backward linkages, formal job absorption, and export competitiveness; addressed via PLI schemes, PM Gati Shakti, National Logistics Policy, and labor code rationalization", "Abolishing all industrial corridors and halting domestic semiconductor fabrication", "Replacing all capital goods production with raw unrefined mineral exports"}, 2, "Core Mains Rubric: Structural transformation (Lewis model), tackling logistics cost (~13-14% GDP) via PM Gati Shakti, Production Linked Incentive (PLI) across 14 sectors.", 150, 920, 690));
        questionBank.push_back(new MainsQuestion(20, 2022, "GS-III (Economy & Security)", "What are the different elements of cyber security? Keeping in view the challenges in cyber security, examine the extent to which India has successfully developed a comprehensive National Cyber Security Strategy. (UPSC Mains GS-III 2022)", {"Application, network, operational, and critical information infrastructure security; strengthened via CERT-In, NCIIPC, Cyber Swachhta Kendra, and DPDP Act, though cross-border attribution gaps remain", "Disconnecting all banking and power grid servers from digital networks permanently", "Delegating national defense firewalls entirely to unverified foreign private contractors", "Ignoring cloud security and focusing exclusively on physical postal mail"}, 1, "Core Mains Rubric: Confidentiality-Integrity-Availability (CIA triad), NCIIPC for critical infrastructure, CERT-In 6-hour reporting mandate, and capacity building.", 150, 895, 650));
        questionBank.push_back(new MainsQuestion(21, 2021, "GS-III (Economy & Security)", "Explain the purpose of the Green Grid Initiative launched at the World Leaders Summit of the COP26 UN Climate Change Conference in Glasgow. (UPSC Mains GS-III 2021)", {"To expand coal-fired thermal power plants across Arctic shipping lanes", "To build an interconnected trans-national transmission grid ('One Sun, One World, One Grid' - OSOWOG) enabling continuous cross-border solar/renewable energy transfer across time zones", "To ban all wind and hydroelectricity generation in developing nations", "To replace electric grids with diesel generators in metropolitan cities"}, 2, "Core Mains Rubric: OSOWOG / Green Grid Initiative overcomes solar intermittency by linking regional power grids across time zones so 'the sun never sets' on the grid.", 150, 910, 675));
        questionBank.push_back(new MainsQuestion(22, 2023, "GS-IV (Ethics)", "What do you understand by 'moral integrity' and 'professional efficiency' in the context of corporate governance in India? (UPSC Mains GS-IV 2023)", {"Maximizing short-term shareholder profit through creative accounting regardless of environmental externalities", "Moral integrity ensures ethical adherence, transparency, and fiduciary honesty, while professional efficiency delivers competent execution; both must converge under SEBI Kotak/CSR norms for sustainable trust", "Professional efficiency requires bypassing statutory audits and whistleblower safeguards", "Moral integrity applies only to non-profit charities and never to commercial enterprises"}, 2, "Core Mains Rubric: Competency without integrity breeds white-collar fraud (e.g., Satyam); integrity without efficiency causes stagnation. Trusteeship + ESG compliance.", 150, 930, 720));
        questionBank.push_back(new MainsQuestion(23, 2022, "GS-IV (Ethics)", "Wisdom lies in knowing what to reckon with and what to overlook. An officer being engrossed with the periphery, ignoring the core issues before him, is not rare in the bureaucracy. Do you agree? (UPSC Mains GS-IV 2022)", {"Civil servants should prioritize rigid procedural red-tape over substantive public welfare outcomes", "Yes; bureaucratic ritualism ('goal displacement' where rules become ends in themselves) sidelines citizen-centric welfare; practical wisdom (Phronesis) requires prioritizing constitutional spirit and core impact", "Officers should ignore statutory laws entirely and act solely on personal impulse", "Routine clerical formatting is more important than disaster relief delivery"}, 2, "Core Mains Rubric: Robert Merton's 'Goal Displacement' vs Aristotelian Phronesis (practical wisdom); distinguishing rule-bound lethargy from outcome-oriented empathy.", 150, 880, 660));
        questionBank.push_back(new MainsQuestion(24, 2020, "GS-IV (Ethics)", "What are the main components of emotional intelligence (EI)? Can they be learned? Discuss. (UPSC Mains GS-IV 2020)", {"EI is a fixed genetic trait determined at birth that cannot be modified through training", "Self-awareness, self-regulation, internal motivation, empathy, and social skills (Daniel Goleman's model); unlike static IQ, EI is neuroplastic and cultivable through mindfulness, active listening, and reflective field training", "Suppression of all human empathy to enforce mechanical administrative orders", "Memorizing legal statutes without interacting with citizens"}, 2, "Core Mains Rubric: Goleman's 5 pillars (Self-Awareness, Self-Regulation, Motivation, Empathy, Social Skills); learned via role-play, sensitivity training, and feedback.", 150, 970, 760));
    }

    int getValidatedInput(int minVal, int maxVal, bool isOptionPrompt = false) {
        while (true) {
            int input;
            if (!(cin >> input)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "[Input Error]: Please enter a valid numeric choice (" << minVal << " to " << maxVal << "): ";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            try {
                if (input < minVal || input > maxVal) {
                    if (isOptionPrompt) throw InvalidOptionException("Option must be between " + to_string(minVal) + " and " + to_string(maxVal) + ".");
                    else throw MenuSelectionException("Selection must be between " + to_string(minVal) + " and " + to_string(maxVal) + ".");
                }
                return input;
            }
            catch (const InvalidOptionException& e) { cout << e.what() << "\nRe-enter option: "; }
            catch (const MenuSelectionException& e) { cout << e.what() << "\nRe-enter selection: "; }
            catch (const UPSCException& e) { cout << e.what() << "\nRe-enter: "; }
        }
    }

    vector<Question*> filterQuestions(bool wantPrelims, const string& subjectFilter) {
        vector<Question*> filtered;
        for (Question* q : questionBank) {
            const PrelimsQuestion* pq = dynamic_cast<const PrelimsQuestion*>(q);
            bool isPrelimsObj = (pq != nullptr);
            if (wantPrelims == isPrelimsObj && (subjectFilter == "ALL" || q->getSubject() == subjectFilter)) {
                filtered.push_back(q);
            }
        }
        return filtered;
    }

public:
    UPSCExamSystem() { initializePYQBank(); }
    ~UPSCExamSystem() { for (Question* q : questionBank) delete q; questionBank.clear(); }

    void runMockTestMode(Candidate& candidate, bool isPrelims, const string& subjectName) {
        vector<Question*> testSet = filterQuestions(isPrelims, subjectName);
        string title = (isPrelims ? "UPSC Prelims - " : "UPSC Mains - ") + (subjectName == "ALL" ? "Full Paper Mock Test" : subjectName + " Mock Test");
        cout << "\n====================================================================\n";
        cout << " STARTING MOCK TEST : " << title << "\n Candidate          : " << candidate.getName() << "\n Total Questions    : " << testSet.size() << "\n";
        cout << (isPrelims ? " Marking Scheme     : +2.00 for Correct | -0.66 for Incorrect | 0 for Skip\n" : " Marking Scheme     : +10.00 for Correct Analytical Framework | 0 for Skip\n");
        cout << " (Note: Answers and Final Scorecard will be revealed at the end of the test)\n====================================================================\n";

        Scorecard card(title, static_cast<int>(testSet.size()));
        vector<int> userResponses;

        for (size_t i = 0; i < testSet.size(); ++i) {
            Question* q = testSet[i];
            cout << "\n[Question " << (i + 1) << " of " << testSet.size() << "]" << *q << "Select your option [1-4 for A-D, or 0 to Skip]: ";
            int choice = getValidatedInput(0, 4, true);
            userResponses.push_back(choice);
            card.recordQuestionResult(choice, q->getCorrectOption(), q->evaluateResponse(choice), q->getMaxMarks());
            if (choice != 0) q->updatePeerStats(choice == q->getCorrectOption());
            cout << " -> Response recorded.\n";
        }
        candidate.incrementTests();

        cout << "\n================ ANSWER KEY & RESPONSE VERIFICATION ================\n";
        for (size_t i = 0; i < testSet.size(); ++i) {
            Question* q = testSet[i];
            int uChoice = userResponses[i], cChoice = q->getCorrectOption();
            cout << "Q" << (i + 1) << " (" << q->getSubject() << ", " << q->getYear() << "): ";
            if (uChoice == 0) cout << "SKIPPED | Correct Option: (" << q->getOptionLetter(cChoice) << ")\n";
            else if (uChoice == cChoice) cout << "CORRECT (" << q->getOptionLetter(uChoice) << ") [+" << q->getMaxMarks() << " Marks]\n";
            else cout << "INCORRECT (You chose " << q->getOptionLetter(uChoice) << " | Correct was " << q->getOptionLetter(cChoice) << ")\n";
        }
        displayAuditSummary(candidate, card);
    }

    void runAttemptQuestionsMode(Candidate& candidate, bool isPrelims, const string& subjectName) {
        vector<Question*> practiceSet = filterQuestions(isPrelims, subjectName);
        cout << "\n====================================================================\n";
        cout << " PRACTICE MODE (INSTANT FEEDBACK) : " << (isPrelims ? "Prelims - " : "Mains - ") << subjectName << "\n";
        cout << " Candidate                        : " << candidate.getName() << "\n Questions in Section             : " << practiceSet.size() << "\n====================================================================\n";

        for (size_t i = 0; i < practiceSet.size(); ++i) {
            Question* q = practiceSet[i];
            cout << "\n[Question " << (i + 1) << " of " << practiceSet.size() << "]" << *q << "Select your option [1-4 for A-D, or 0 to Skip]: ";
            int choice = getValidatedInput(0, 4, true), corrOpt = q->getCorrectOption();

            cout << "\n>>> [INSTANT ANSWER VERIFICATION] <<<\n";
            if (choice == 0) {
                cout << "Status               : SKIPPED (Unattempted)\n";
                cout << "Automatic Correct Ans: Option [" << corrOpt << "] (" << q->getOptionLetter(corrOpt) << ") " << q->getOptionText(corrOpt) << "\n";
            } else if (choice == corrOpt) {
                q->updatePeerStats(true);
                cout << "Status               : CORRECT ANSWER!\n";
                cout << "Selected Option      : Option [" << choice << "] (" << q->getOptionLetter(choice) << ") " << q->getOptionText(choice) << "\n";
            } else {
                q->updatePeerStats(false);
                cout << "Status               : INCORRECT SELECTION!\n";
                cout << "Your Incorrect Option: Option [" << choice << "] (" << q->getOptionLetter(choice) << ") " << q->getOptionText(choice) << "\n";
                cout << "Automatic Correct Ans: Option [" << corrOpt << "] (" << q->getOptionLetter(corrOpt) << ") " << q->getOptionText(corrOpt) << "\n";
            }
            cout << fixed << setprecision(2) << "Explanation          : " << q->getExplanation() << "\n";
            cout << "Live Peer Accuracy   : " << q->getPeerAccuracy() << "% of aspirants got this right.\n--------------------------------------------------------------------\n";
        }
    }

    string selectSubjectMenu(bool isPrelims, bool allowFullPaper) {
        cout << "\nSelect Subject:\n";
        if (isPrelims) {
            cout << "  [1] Indian Polity (3 PYQs)\n  [2] Indian Economy (3 PYQs)\n  [3] Environment & History (3 PYQs)\n  [4] CSAT Aptitude (3 PYQs)\n";
            if (allowFullPaper) cout << "  [5] Full Prelims Subject Paper (All 12 PYQs)\n";
            cout << "Enter choice: ";
            int subChoice = getValidatedInput(1, allowFullPaper ? 5 : 4);
            switch (subChoice) { case 1: return "Polity"; case 2: return "Economy"; case 3: return "Environment & History"; case 4: return "CSAT"; default: return "ALL"; }
        } else {
            cout << "  [1] GS-I (History, Culture & Society - 3 PYQs)\n  [2] GS-II (Polity, Governance & IR - 3 PYQs)\n  [3] GS-III (Economy & Internal Security - 3 PYQs)\n  [4] GS-IV (Ethics, Integrity & Aptitude - 3 PYQs)\n";
            if (allowFullPaper) cout << "  [5] Full Mains Paper (All 12 PYQs)\n";
            cout << "Enter choice: ";
            int subChoice = getValidatedInput(1, allowFullPaper ? 5 : 4);
            switch (subChoice) { case 1: return "GS-I (History & Society)"; case 2: return "GS-II (Polity & IR)"; case 3: return "GS-III (Economy & Security)"; case 4: return "GS-IV (Ethics)"; default: return "ALL"; }
        }
    }

    void startPortal() {
        cout << "====================================================================\n";
        cout << "        WELCOME TO THE UPSC CSE PYQ MOCK & PRACTICE PORTAL          \n";
        cout << "        (Total Official UPSC PYQs Loaded in Bank: " << Question::getTotalQuestionsLoaded() << " Questions)      \n";
        cout << "====================================================================\nPlease enter your full name: ";
        string userName;
        getline(cin, userName);
        if (userName.empty()) userName = "UPSC Aspirant";
        Candidate candidate(userName);
        cout << "\nWelcome, " << candidate.getName() << "!\n";

        bool keepRunning = true;
        while (keepRunning) {
            cout << "\n====================================================================\n";
            cout << "MAIN STARTING OPTIONS (Candidate: " << candidate.getName() << ")\n====================================================================\n";
            cout << "Which examination stage do you want to attempt?\n  [1] UPSC Prelims\n  [2] UPSC Mains\nEnter your choice (1 or 2): ";
            bool isPrelims = (getValidatedInput(1, 2) == 1);

            cout << "\nSelect Mode for " << (isPrelims ? "UPSC Prelims" : "UPSC Mains") << ":\n";
            cout << "  [1] Give Mock Test (Questions one-by-one, Scorecard at the end)\n  [2] Attempt Questions (Subject-wise with Instant Answer Feedback)\nEnter your choice (1 or 2): ";
            int modeChoice = getValidatedInput(1, 2);

            if (modeChoice == 1) runMockTestMode(candidate, isPrelims, selectSubjectMenu(isPrelims, true));
            else runAttemptQuestionsMode(candidate, isPrelims, selectSubjectMenu(isPrelims, false));

            cout << "\n====================================================================\nWhat would you like to do next?\n";
            cout << "  [1] Go back to Starting Options (Prelims / Mains Menu)\n  [2] End and Exit Portal\nEnter your choice (1 or 2): ";
            if (getValidatedInput(1, 2) == 2) keepRunning = false;
        }
        cout << "\n====================================================================\n";
        cout << "Thank you, " << candidate.getName() << "! Total Mock Tests Completed: " << candidate.getTestsCompleted() << "\n";
        cout << "All the best for your UPSC Civil Services Examination!\n====================================================================\n";
    }
};

int main() {
    UPSCExamSystem app;
    app.startPortal();
    return 0;
}