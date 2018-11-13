#include "SinglePhoton_module.h"


namespace single_photon
{
    void SinglePhoton::ClearShowers(){
        m_reco_asso_showers=0;
        m_reco_shower_startx.clear();
        m_reco_shower_starty.clear();
        m_reco_shower_startz.clear();
        m_reco_shower_dirx.clear();
        m_reco_shower_diry.clear();
        m_reco_shower_dirz.clear();
        m_reco_shower_theta_yz.clear();
        m_reco_shower_phi_yx.clear();

        m_reco_shower_conversion_distance.clear();

        m_reco_shower_openingangle.clear();
        m_reco_shower_length.clear();

        m_reco_shower_delaunay_num_triangles_plane0.clear();
        m_reco_shower_delaunay_num_triangles_plane1.clear();
        m_reco_shower_delaunay_num_triangles_plane2.clear();
        m_reco_shower_num_hits_plane0.clear();
        m_reco_shower_num_hits_plane1.clear();
        m_reco_shower_num_hits_plane2.clear();

        m_reco_shower_delaunay_area_plane0.clear();
        m_reco_shower_delaunay_area_plane1.clear();
        m_reco_shower_delaunay_area_plane2.clear();

    }

    void SinglePhoton::ResizeShowers(size_t size){
        m_reco_shower_startx.resize(size);
        m_reco_shower_starty.resize(size);
        m_reco_shower_startz.resize(size);
        m_reco_shower_dirx.resize(size);
        m_reco_shower_diry.resize(size);
        m_reco_shower_dirz.resize(size);
         m_reco_shower_theta_yz.resize(size);
        m_reco_shower_phi_yx.resize(size);


        m_reco_shower_conversion_distance.resize(size);

        m_reco_shower_openingangle.resize(size);
        m_reco_shower_length.resize(size);

        m_reco_shower_delaunay_num_triangles_plane0.resize(size);
        m_reco_shower_delaunay_num_triangles_plane1.resize(size);
        m_reco_shower_delaunay_num_triangles_plane2.resize(size);
        m_reco_shower_num_hits_plane0.resize(size);
        m_reco_shower_num_hits_plane1.resize(size);
        m_reco_shower_num_hits_plane2.resize(size);


        m_reco_shower_delaunay_area_plane0.resize(size);
        m_reco_shower_delaunay_area_plane1.resize(size);
        m_reco_shower_delaunay_area_plane2.resize(size);


    }

    void SinglePhoton::CreateShowerBranches(){
        vertex_tree->Branch("reco_asso_showers",&m_reco_asso_showers,"reco_asso_showers/I");
        vertex_tree->Branch("reco_shower_length", &m_reco_shower_length);
        vertex_tree->Branch("reco_shower_opening_angle", &m_reco_shower_openingangle);
        vertex_tree->Branch("reco_shower_dirx", &m_reco_shower_dirx);
        vertex_tree->Branch("reco_shower_diry", &m_reco_shower_diry);
        vertex_tree->Branch("reco_shower_dirz", &m_reco_shower_dirz);
        vertex_tree->Branch("reco_shower_startx", &m_reco_shower_startx);
        vertex_tree->Branch("reco_shower_starty", &m_reco_shower_starty);
        vertex_tree->Branch("reco_shower_startz", &m_reco_shower_startz);
        vertex_tree->Branch("reco_shower_theta_yz",&m_reco_shower_theta_yz);
        vertex_tree->Branch("reco_shower_phi_yx",&m_reco_shower_phi_yx);


        vertex_tree->Branch("reco_shower_conversion_distance",& m_reco_shower_conversion_distance);

        vertex_tree->Branch("reco_shower_delaunay_num_triangles_plane0",&m_reco_shower_delaunay_num_triangles_plane0);
        vertex_tree->Branch("reco_shower_delaunay_num_triangles_plane1",&m_reco_shower_delaunay_num_triangles_plane1);
        vertex_tree->Branch("reco_shower_delaunay_num_triangles_plane2",&m_reco_shower_delaunay_num_triangles_plane2);
        vertex_tree->Branch("reco_shower_num_hits_plane0",&m_reco_shower_num_hits_plane0);
        vertex_tree->Branch("reco_shower_num_hits_plane1",&m_reco_shower_num_hits_plane1);
        vertex_tree->Branch("reco_shower_num_hits_plane2",&m_reco_shower_num_hits_plane2);
        
        vertex_tree->Branch("reco_shower_delaunay_area_plane0",&m_reco_shower_delaunay_area_plane0);
        vertex_tree->Branch("reco_shower_delaunay_area_plane1",&m_reco_shower_delaunay_area_plane1);
        vertex_tree->Branch("reco_shower_delaunay_area_plane2",&m_reco_shower_delaunay_area_plane2);

    }

    void SinglePhoton::AnalyzeShowers(const std::vector<art::Ptr<recob::Shower>>& showers,  std::map<art::Ptr<recob::Shower>,art::Ptr<recob::PFParticle>> & showerToPFParticleMap, std::map<art::Ptr<recob::PFParticle>, std::vector<art::Ptr<recob::Hit>>> & pfParticleToHitMap){

        if(m_is_verbose) std::cout<<"SinglePhoton::AnalyzeShowers()\t||\t Begininning recob::Shower analysis suite\n";
            
        m_reco_asso_showers=showers.size();
        int i_shr = 0;
        this->ResizeShowers(m_reco_asso_showers);

        for (ShowerVector::const_iterator iter = showers.begin(), iterEnd = showers.end(); iter != iterEnd; ++iter)
        {

            const art::Ptr<recob::Shower> shower = *iter;
            const art::Ptr<recob::PFParticle> pfp = showerToPFParticleMap[shower];
            const std::vector<art::Ptr<recob::Hit>> hits =  pfParticleToHitMap[pfp];

            //int m_shrid = shower->ID(); This is an used variable, always -999
            double m_length = shower->Length();
            double m_open_angle = shower->OpenAngle();

            TVector3 shr_start = shower->ShowerStart();
            TVector3 shr_dir = shower->Direction();

            if(m_is_verbose) std::cout<<"SinglePhoton::AnalyzeShowers()\t||\t On Shower: "<<i_shr<<" which has length: "<<m_length<<"\n";
          
            m_reco_shower_startx[i_shr] = shr_start.X();
            m_reco_shower_starty[i_shr] = shr_start.Y();
            m_reco_shower_startz[i_shr] = shr_start.Z();

            m_reco_shower_dirx[i_shr] = shr_dir.X();
            m_reco_shower_diry[i_shr] = shr_dir.Y();
            m_reco_shower_dirz[i_shr] = shr_dir.Z();

            m_reco_shower_length[i_shr] = m_length;
            m_reco_shower_openingangle[i_shr] = m_open_angle;

            m_reco_shower_conversion_distance[i_shr] = sqrt( pow(shr_start.X()-m_vertex_pos_x,2)+pow(shr_start.Y()-m_vertex_pos_y,2)+ pow(shr_start.Z()-m_vertex_pos_z,2)  );
 
            m_reco_shower_theta_yz[i_shr] = atan2(m_reco_shower_diry[i_shr],m_reco_shower_dirz[i_shr]);
            m_reco_shower_phi_yx[i_shr] = atan2(m_reco_shower_diry[i_shr],m_reco_shower_dirx[i_shr]);


            std::vector<int> t_num(3,0);
            std::vector<int> t_numhits(3,0);
            std::vector<double> t_area(3,0.0);
            //Right, this basically loops over all hits in all planes and for each plane forms the Delaunay triangilization of it and calculates the 2D area inscribed by the convex hull
            
            if(m_is_verbose) std::cout<<"SinglePhoton::AnalyzeShowers()\t||\t Starting Delaunay Triangleization\n";
          
            auto start = std::chrono::high_resolution_clock::now();
            this->delaunay_hit_wrapper(hits, t_numhits, t_num, t_area);
            
            auto finish = std::chrono::high_resolution_clock::now();
            auto microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start);
            if(m_is_verbose) std::cout<<"SinglePhoton::AnalyzeShowers()\t||\t Finished Delaunay Triangleization. It took "<< microseconds.count() << "ms and found "<<t_num[0]+t_num[1]+t_num[2]<<" triangles\n";

            m_reco_shower_delaunay_num_triangles_plane0[i_shr] = t_num[0];
            m_reco_shower_delaunay_num_triangles_plane1[i_shr] = t_num[1];
            m_reco_shower_delaunay_num_triangles_plane2[i_shr] = t_num[2];

            m_reco_shower_delaunay_area_plane0[i_shr] = t_area[0];
            m_reco_shower_delaunay_area_plane1[i_shr] = t_area[1];
            m_reco_shower_delaunay_area_plane2[i_shr] = t_area[2];

            m_reco_shower_num_hits_plane0[i_shr] = t_numhits[0];
            m_reco_shower_num_hits_plane1[i_shr] = t_numhits[1];
            m_reco_shower_num_hits_plane2[i_shr] = t_numhits[2];


            //-------------- Flashes : Was there a flash in the beam_time and if so was it near in Z? --------------------
            double zmin = m_reco_shower_startz[i_shr];
            double zmax = zmin + m_reco_shower_dirz[i_shr]*m_reco_shower_length[i_shr];
            if(zmin > zmax) std::swap(zmin, zmax);

            double ymin = m_reco_shower_starty[i_shr];
            double ymax = zmin + m_reco_shower_diry[i_shr]*m_reco_shower_length[i_shr];
            if(ymin > ymax) std::swap(ymin, ymax);

            //Now loop over all flashes (only in beamtime) and find SOMETHING?
	    //Code property of Gray Yarbrough (all rights reserved)
	    int optical_flash_in_beamgate_counter=0;
	    double shortest_distance_to_beamgate=DBL_MAX;
	    for(size_t i_flash = 0; i_flash < m_reco_numflashes; ++i_flash) {
	      double const flash_time=m_reco_flash_time[i_flash];
	      double const zcenter=m_reco_flash_zcenter[i_flash];
	      double const ycenter=m_reco_flash_ycenter[i_flash];
	      //if(fverbose) std::cout<<"optical flash time"<< m_reco_flash_time[i_flash] << " (opf.Time()>3.2 && opf.Time<4.8)\n
	      double dist=DBL_MAX;
	       if(zcenter < zmin) {
		 dist = zmin - zcenter;
		 if(fverbose) std::cout << "\z flash center - zmin dist: " << dist << "\n";
    }
	       else if(zcenter > zmax) {
		 dist = zcenter - zmax;
		 if(fverbose) std::cout << "\z flash center - zmax dist: " << dist << "\n";
	       } 
	       else {
		 dist = 0;
		 if(fverbose) std::cout << "\z flash center inside shower\n";
	       }
	       if(dist < shortest_dist) shortest_dist = dist;
  }
	    if(fverbose) std::cout << "\tshortest_dist: " << shortest_dist << "\n";
	    //assume setting to nonsense value
	    if(m_reco_num_flashes_in_beamgate == 0) shortest_dist = -2;
	    std::cout<<"flash shortist distance: "<< shortest_dist;

	    //end optical flash code
            i_shr++;
        }


           if(m_is_verbose) std::cout<<"SinglePhoton::AnalyzeShowers()\t||\t Finished.\n";
    }

    //-----------------------------------------------------------------------------------------------------------------------------------------

    void SinglePhoton::CollectCalo(const art::Event &evt, const art::Ptr<recob::Shower> &shower)
    {

        art::ValidHandle< std::vector<recob::Shower>> theShowers = evt.getValidHandle<std::vector<recob::Shower>>(m_showerLabel);
        if (!theShowers.isValid())
        {
            mf::LogDebug("LArPandora") << "  Failed to find Shower Information... " << "\n";
            return;
        }
        else
        {
            mf::LogDebug("LArPandora") << "  Found: " << theShowers->size() << " Showers " << "\n";
        }
        art::FindManyP<anab::Calorimetry> theCaloAssns(theShowers, evt, m_caloLabel);



        for (unsigned int i = 0; i < theShowers->size(); ++i)
        {
            const art::Ptr<recob::Shower> tmp_shower(theShowers, i);
            const std::vector< art::Ptr<anab::Calorimetry> > calo = theCaloAssns.at(i);

            if(tmp_shower == shower){

                break;
            }
        }

    }


}
