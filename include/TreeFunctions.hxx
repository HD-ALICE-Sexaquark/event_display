#ifndef TREE_FUNCTIONS_HXX
#define TREE_FUNCTIONS_HXX

//________________________________________________________________________
struct Event_tt {
    //
    // This is the structure of the output TTree
    // [based on AliAnalysisTaskSexaquark.hxx]
    //
    /* MC particles */                           //
    Int_t N_MCGen;                               // number of MC particles
    std::vector<Float_t>* MC_Px = 0;             // x-component of true momentum
    std::vector<Float_t>* MC_Py = 0;             // y-component of true momentum
    std::vector<Float_t>* MC_Pz = 0;             // z-component of true momentum
    std::vector<Float_t>* MC_X = 0;              // x-coordinate of generation vertex
    std::vector<Float_t>* MC_Y = 0;              // y-coordinate of generation vertex
    std::vector<Float_t>* MC_Z = 0;              // z-coordinate of generation vertex
    std::vector<Float_t>* MC_Xf = 0;             //
    std::vector<Float_t>* MC_Yf = 0;             //
    std::vector<Float_t>* MC_Zf = 0;             //
    std::vector<Int_t>* MC_PID = 0;              // PDG code
    std::vector<Int_t>* MC_Mother = 0;           // index of mother
    std::vector<Int_t>* MC_PID_Mother = 0;       // PID of mother
    std::vector<Int_t>* MC_PID_GrandMother = 0;  // PID of grandmother
    std::vector<Int_t>* MC_NDaughters = 0;       //
    std::vector<Int_t>* MC_FirstDau = 0;         // index of first daughter
    std::vector<Int_t>* MC_LastDau = 0;          // index of last daughter
    std::vector<Int_t>* MC_Status = 0;           // MC status code
    std::vector<Bool_t>* MC_isSignal = 0;        // kTRUE if it belongs to anti-sexaquark signal, kFALSE if background
    /* MC Rec. Tracks */                         //
    Int_t N_MCRec;                               // number of MC reconstructed tracks
    std::vector<Int_t>* Idx_True = 0;            // index of true MC particle
    std::vector<Float_t>* Rec_Px = 0;            // x-component of reconstructed momentum
    std::vector<Float_t>* Rec_Py = 0;            // y-component of reconstructed momentum
    std::vector<Float_t>* Rec_Pz = 0;            // z-component of reconstructed momentum
    std::vector<Short_t>* Rec_Charge = 0;        // measured charge
    std::vector<Float_t>* Rec_IP_wrtPV = 0;      // transverse impact parameter w.r.t. Primary Vertex
    std::vector<Float_t>* Rec_NSigmaPion = 0;    // absolute value of likeness to be a charged pion (closer to 0, the most likely)
    std::vector<Float_t>* Rec_NSigmaProton = 0;  // absolute value of likeness to be a proton (closer to 0, the most likely)
    std::vector<Short_t>* Rec_NClustersTPC = 0;  // number of clusters assigned in the TPC
    std::vector<Bool_t>* Rec_isDuplicate = 0;    // kTRUE if track is a duplicate, kFALSE if not (v1: compared to true particle)
    std::vector<Bool_t>* Rec_isSimilar = 0;      // kTRUE if track is a duplicate, kFALSE if not (v2: compared to similar particles)
    std::vector<Bool_t>* Rec_isSignal = 0;       // kTRUE if it belongs to anti-sexaquark signal, kFALSE if background
    std::vector<Float_t>* Rec_HelixParam0 = 0;   //
    std::vector<Float_t>* Rec_HelixParam1 = 0;   //
    std::vector<Float_t>* Rec_HelixParam2 = 0;   //
    std::vector<Float_t>* Rec_HelixParam3 = 0;   //
    std::vector<Float_t>* Rec_HelixParam4 = 0;   //
    std::vector<Float_t>* Rec_HelixParam5 = 0;   //
    /* V0s */                                    //
    Int_t N_V0s;                                 // number of formed V0s
    std::vector<Int_t>* Idx_Pos = 0;             // index of positive daughter
    std::vector<Int_t>* Idx_Neg = 0;             // index of negative daughter
    std::vector<Float_t>* V0_Px = 0;             // x-component of V0 momentum
    std::vector<Float_t>* V0_Py = 0;             // y-component of V0 momentum
    std::vector<Float_t>* V0_Pz = 0;             // z-component of V0 momentum
    std::vector<Float_t>* V0_X = 0;              // x-coordinate of V0
    std::vector<Float_t>* V0_Y = 0;              // y-coordinate of V0
    std::vector<Float_t>* V0_Z = 0;              // z-coordinate of V0
    std::vector<Float_t>* Pos_Px = 0;            // x-component of positive track momentum at V0 position
    std::vector<Float_t>* Pos_Py = 0;            // y-component of positive track momentum at V0 position
    std::vector<Float_t>* Pos_Pz = 0;            // z-component of positive track momentum at V0 position
    std::vector<Float_t>* Neg_Px = 0;            // x-component of negative track momentum at V0 position
    std::vector<Float_t>* Neg_Py = 0;            // y-component of negative track momentum at V0 position
    std::vector<Float_t>* Neg_Pz = 0;            // z-component of negative track momentum at V0 position
    std::vector<Bool_t>* V0_isSignal = 0;        // kTRUE if signal, kFALSE if background
    std::vector<Float_t>* V0_E_asK0 = 0;         // energy of V0, assuming it's a K0
    std::vector<Float_t>* V0_E_asAL = 0;         // energy of V0, assuming it's an anti-lambda
    std::vector<Bool_t>* V0_couldBeK0 = 0;       // kTRUE if K0 candidate, kFALSE if not
    std::vector<Bool_t>* V0_couldBeAL = 0;       // kTRUE if anti-lambda candidate, kFALSE if not
    std::vector<Float_t>* V0_Chi2 = 0;           // chi2 value from the Kalman Filter (?)
    std::vector<Float_t>* V0_DCA_Daughters = 0;  // distance of closest approach between daughters
    std::vector<Float_t>* V0_IP_wrtPV = 0;       // impact parameter w.r.t. Primary Vertex
    std::vector<Float_t>* V0_CPA_wrtPV = 0;      // cosine of pointing angle w.r.t. Primary Vertex
    std::vector<Float_t>* V0_ArmAlpha = 0;       // Armenteros-Podolanski variable alpha
    std::vector<Float_t>* V0_ArmPt = 0;          // Armenteros-Podolanski variable Pt
    std::vector<Float_t>* V0_DecayLength = 0;    // distance between PV and V0
    std::vector<Float_t>* V0_DCA_wrtPV = 0;      // distance of closest approach to Primary Vertex
};

//________________________________________________________________________
void LoadBranches(TTree* this_tree, Event_tt& this_event) {
    /* MC particles */
    this_tree->SetBranchAddress("N_MCGen", &this_event.N_MCGen);
    this_tree->SetBranchAddress("MC_Px", &this_event.MC_Px);
    this_tree->SetBranchAddress("MC_Py", &this_event.MC_Py);
    this_tree->SetBranchAddress("MC_Pz", &this_event.MC_Pz);
    this_tree->SetBranchAddress("MC_X", &this_event.MC_X);
    this_tree->SetBranchAddress("MC_Y", &this_event.MC_Y);
    this_tree->SetBranchAddress("MC_Z", &this_event.MC_Z);
    this_tree->SetBranchAddress("MC_Xf", &this_event.MC_Xf);
    this_tree->SetBranchAddress("MC_Yf", &this_event.MC_Yf);
    this_tree->SetBranchAddress("MC_Zf", &this_event.MC_Zf);
    this_tree->SetBranchAddress("MC_PID", &this_event.MC_PID);
    this_tree->SetBranchAddress("MC_Mother", &this_event.MC_Mother);
    this_tree->SetBranchAddress("MC_NDaughters", &this_event.MC_NDaughters);
    this_tree->SetBranchAddress("MC_FirstDau", &this_event.MC_FirstDau);
    this_tree->SetBranchAddress("MC_LastDau", &this_event.MC_LastDau);
    this_tree->SetBranchAddress("MC_Status", &this_event.MC_Status);
    this_tree->SetBranchAddress("MC_isSignal", &this_event.MC_isSignal);
    /* MC Rec Tracks */
    this_tree->SetBranchAddress("N_MCRec", &this_event.N_MCRec);
    this_tree->SetBranchAddress("Idx_True", &this_event.Idx_True);
    this_tree->SetBranchAddress("Rec_Px", &this_event.Rec_Px);
    this_tree->SetBranchAddress("Rec_Py", &this_event.Rec_Py);
    this_tree->SetBranchAddress("Rec_Pz", &this_event.Rec_Pz);
    this_tree->SetBranchAddress("Rec_Charge", &this_event.Rec_Charge);
    this_tree->SetBranchAddress("Rec_IP_wrtPV", &this_event.Rec_IP_wrtPV);
    this_tree->SetBranchAddress("Rec_NSigmaPion", &this_event.Rec_NSigmaPion);
    this_tree->SetBranchAddress("Rec_NSigmaProton", &this_event.Rec_NSigmaProton);
    this_tree->SetBranchAddress("Rec_NClustersTPC", &this_event.Rec_NClustersTPC);
    this_tree->SetBranchAddress("Rec_isDuplicate", &this_event.Rec_isDuplicate);
    this_tree->SetBranchAddress("Rec_isSimilar", &this_event.Rec_isSimilar);
    this_tree->SetBranchAddress("Rec_isSignal", &this_event.Rec_isSignal);
    this_tree->SetBranchAddress("Rec_HelixParam0", &this_event.Rec_HelixParam0);
    this_tree->SetBranchAddress("Rec_HelixParam1", &this_event.Rec_HelixParam1);
    this_tree->SetBranchAddress("Rec_HelixParam2", &this_event.Rec_HelixParam2);
    this_tree->SetBranchAddress("Rec_HelixParam3", &this_event.Rec_HelixParam3);
    this_tree->SetBranchAddress("Rec_HelixParam4", &this_event.Rec_HelixParam4);
    this_tree->SetBranchAddress("Rec_HelixParam5", &this_event.Rec_HelixParam5);
    /* V0s */
    this_tree->SetBranchAddress("N_V0s", &this_event.N_V0s);
    this_tree->SetBranchAddress("Idx_Pos", &this_event.Idx_Pos);
    this_tree->SetBranchAddress("Idx_Neg", &this_event.Idx_Neg);
    this_tree->SetBranchAddress("V0_Px", &this_event.V0_Px);
    this_tree->SetBranchAddress("V0_Py", &this_event.V0_Py);
    this_tree->SetBranchAddress("V0_Pz", &this_event.V0_Pz);
    this_tree->SetBranchAddress("V0_X", &this_event.V0_X);
    this_tree->SetBranchAddress("V0_Y", &this_event.V0_Y);
    this_tree->SetBranchAddress("V0_Z", &this_event.V0_Z);
    this_tree->SetBranchAddress("Pos_Px", &this_event.Pos_Px);
    this_tree->SetBranchAddress("Pos_Py", &this_event.Pos_Py);
    this_tree->SetBranchAddress("Pos_Pz", &this_event.Pos_Pz);
    this_tree->SetBranchAddress("Neg_Px", &this_event.Neg_Px);
    this_tree->SetBranchAddress("Neg_Py", &this_event.Neg_Py);
    this_tree->SetBranchAddress("Neg_Pz", &this_event.Neg_Pz);
    this_tree->SetBranchAddress("V0_isSignal", &this_event.V0_isSignal);
    this_tree->SetBranchAddress("V0_E_asK0", &this_event.V0_E_asK0);
    this_tree->SetBranchAddress("V0_E_asAL", &this_event.V0_E_asAL);
    this_tree->SetBranchAddress("V0_couldBeK0", &this_event.V0_couldBeK0);
    this_tree->SetBranchAddress("V0_couldBeAL", &this_event.V0_couldBeAL);
    this_tree->SetBranchAddress("V0_Chi2", &this_event.V0_Chi2);
    this_tree->SetBranchAddress("V0_DCA_Daughters", &this_event.V0_DCA_Daughters);
    this_tree->SetBranchAddress("V0_IP_wrtPV", &this_event.V0_IP_wrtPV);
    this_tree->SetBranchAddress("V0_CPA_wrtPV", &this_event.V0_CPA_wrtPV);
    this_tree->SetBranchAddress("V0_ArmAlpha", &this_event.V0_ArmAlpha);
    this_tree->SetBranchAddress("V0_ArmPt", &this_event.V0_ArmPt);
    this_tree->SetBranchAddress("V0_DecayLength", &this_event.V0_DecayLength);
    this_tree->SetBranchAddress("V0_DCA_wrtPV", &this_event.V0_DCA_wrtPV);
}

#endif
