/**********************************************
  Copyright Mentor Graphics Corporation 2018

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#pragma once

//////////////////////////////////////////////////////////////////////////

#define DEFINE_RPR_XML_TAG(p)	static const wchar_ngp* const tag_##p = L ## #p

//////////////////////////////////////////////////////////////////////////

//common
DEFINE_RPR_XML_TAG( align );
DEFINE_RPR_XML_TAG( ambient );
DEFINE_RPR_XML_TAG( arrow_quality );
DEFINE_RPR_XML_TAG( arrow_size );
DEFINE_RPR_XML_TAG( box );
DEFINE_RPR_XML_TAG( callout );
DEFINE_RPR_XML_TAG( callouts );
DEFINE_RPR_XML_TAG( camera );
DEFINE_RPR_XML_TAG( cd_box );
DEFINE_RPR_XML_TAG( cd_symmetry );
DEFINE_RPR_XML_TAG( cd_periodicity );
DEFINE_RPR_XML_TAG( cd_dim );
DEFINE_RPR_XML_TAG( is_sector_periodicity );
DEFINE_RPR_XML_TAG( sector_axis );
DEFINE_RPR_XML_TAG( sector_origin );
DEFINE_RPR_XML_TAG( sector_reference_dir );
DEFINE_RPR_XML_TAG( cpt );
DEFINE_RPR_XML_TAG( cfld );
DEFINE_RPR_XML_TAG( char_array );
DEFINE_RPR_XML_TAG( color );
DEFINE_RPR_XML_TAG( color_by_param );
DEFINE_RPR_XML_TAG( components );
DEFINE_RPR_XML_TAG( config_name );
DEFINE_RPR_XML_TAG( cs_name );
DEFINE_RPR_XML_TAG( cs_uuid );
DEFINE_RPR_XML_TAG( cs_xform );
DEFINE_RPR_XML_TAG( default_font );
DEFINE_RPR_XML_TAG( diffuse );
DEFINE_RPR_XML_TAG( display );
DEFINE_RPR_XML_TAG( distance );
DEFINE_RPR_XML_TAG( dynamic );
DEFINE_RPR_XML_TAG( enabled );
DEFINE_RPR_XML_TAG( end );
DEFINE_RPR_XML_TAG( flow_space_type );
DEFINE_RPR_XML_TAG( face_index );
DEFINE_RPR_XML_TAG( geom_index );
DEFINE_RPR_XML_TAG( problem_type );
DEFINE_RPR_XML_TAG( grid );
DEFINE_RPR_XML_TAG( grid_step );
DEFINE_RPR_XML_TAG( height );
DEFINE_RPR_XML_TAG( icon );
DEFINE_RPR_XML_TAG( is_initialized );
DEFINE_RPR_XML_TAG( is_global );
DEFINE_RPR_XML_TAG( is_plot );
DEFINE_RPR_XML_TAG( iteration );
DEFINE_RPR_XML_TAG( length );
DEFINE_RPR_XML_TAG( light_sources );
DEFINE_RPR_XML_TAG( locality );
DEFINE_RPR_XML_TAG( locked_to_model );
DEFINE_RPR_XML_TAG( matrix );
DEFINE_RPR_XML_TAG( max );
DEFINE_RPR_XML_TAG( min );
DEFINE_RPR_XML_TAG( min_grid_step );
DEFINE_RPR_XML_TAG( model_appearance );
DEFINE_RPR_XML_TAG( model_box );
DEFINE_RPR_XML_TAG( model_dim );
DEFINE_RPR_XML_TAG( model_dir );
DEFINE_RPR_XML_TAG( model_name );
DEFINE_RPR_XML_TAG( name );
DEFINE_RPR_XML_TAG( names_width );
DEFINE_RPR_XML_TAG( ncolors );
DEFINE_RPR_XML_TAG( ngp_platform_version );
DEFINE_RPR_XML_TAG( normal );
DEFINE_RPR_XML_TAG( number_of_sectors );
DEFINE_RPR_XML_TAG( orientation );
DEFINE_RPR_XML_TAG( palette );
DEFINE_RPR_XML_TAG( plane_irr_shift );
DEFINE_RPR_XML_TAG( plots );
DEFINE_RPR_XML_TAG( plots_visibility );
DEFINE_RPR_XML_TAG( porous );
DEFINE_RPR_XML_TAG( pos );
DEFINE_RPR_XML_TAG( project_dir );
DEFINE_RPR_XML_TAG( project_name );
DEFINE_RPR_XML_TAG( rotation_type );
DEFINE_RPR_XML_TAG( row_height );
DEFINE_RPR_XML_TAG( scale );
DEFINE_RPR_XML_TAG( size );
DEFINE_RPR_XML_TAG( sketch_color );
DEFINE_RPR_XML_TAG( spacing );
DEFINE_RPR_XML_TAG( specular );
DEFINE_RPR_XML_TAG( start_angle );
DEFINE_RPR_XML_TAG( style );
DEFINE_RPR_XML_TAG( time );
DEFINE_RPR_XML_TAG( title );
DEFINE_RPR_XML_TAG( title_source );
DEFINE_RPR_XML_TAG( type );
DEFINE_RPR_XML_TAG( feature_type );
DEFINE_RPR_XML_TAG( transparency );
DEFINE_RPR_XML_TAG( transient );
DEFINE_RPR_XML_TAG( heat_transfer );
DEFINE_RPR_XML_TAG( use_indexes );
DEFINE_RPR_XML_TAG( uuid );
DEFINE_RPR_XML_TAG( parent_uuid );
DEFINE_RPR_XML_TAG( value );
DEFINE_RPR_XML_TAG( values );
DEFINE_RPR_XML_TAG( values_width );
DEFINE_RPR_XML_TAG( vector );
DEFINE_RPR_XML_TAG( vector_style );
DEFINE_RPR_XML_TAG( version );
DEFINE_RPR_XML_TAG( visibility );
DEFINE_RPR_XML_TAG( selected );
DEFINE_RPR_XML_TAG( vwr_res_files_dir );
DEFINE_RPR_XML_TAG( width );
DEFINE_RPR_XML_TAG( crop_region );
DEFINE_RPR_XML_TAG( crop_region_anchors );
DEFINE_RPR_XML_TAG( id );
DEFINE_RPR_XML_TAG( id1 );
DEFINE_RPR_XML_TAG( id2 );
DEFINE_RPR_XML_TAG( p1 );
DEFINE_RPR_XML_TAG( p2 );
DEFINE_RPR_XML_TAG( up );
DEFINE_RPR_XML_TAG( update_stamp );
DEFINE_RPR_XML_TAG( group );
DEFINE_RPR_XML_TAG( appearance );
DEFINE_RPR_XML_TAG( background_color );
DEFINE_RPR_XML_TAG( scale_color );
DEFINE_RPR_XML_TAG( vals_def_type );
DEFINE_RPR_XML_TAG( value_color );
DEFINE_RPR_XML_TAG( title_color );
DEFINE_RPR_XML_TAG( line_color );
DEFINE_RPR_XML_TAG( auto_line_color );
DEFINE_RPR_XML_TAG( auto_line_color_color );
DEFINE_RPR_XML_TAG( pinned );
DEFINE_RPR_XML_TAG( show_results );
DEFINE_RPR_XML_TAG( min_callout );
DEFINE_RPR_XML_TAG( max_callout );
DEFINE_RPR_XML_TAG( coordinates );
DEFINE_RPR_XML_TAG( position_initialized );
DEFINE_RPR_XML_TAG( position );
DEFINE_RPR_XML_TAG( 2d_position );
DEFINE_RPR_XML_TAG( min_heat_transfer_rate );
DEFINE_RPR_XML_TAG( max_arrow_width );
DEFINE_RPR_XML_TAG( min_arrow_width );
DEFINE_RPR_XML_TAG( min_radius );
DEFINE_RPR_XML_TAG( max_radius );
DEFINE_RPR_XML_TAG( node );
DEFINE_RPR_XML_TAG( nodes );
DEFINE_RPR_XML_TAG( graph );
DEFINE_RPR_XML_TAG( layout );
DEFINE_RPR_XML_TAG( edge_appearance );
DEFINE_RPR_XML_TAG( default_name );
DEFINE_RPR_XML_TAG( show_nodes );
DEFINE_RPR_XML_TAG( periodic );
DEFINE_RPR_XML_TAG( geom_stamp_uuid );
DEFINE_RPR_XML_TAG( unit_length );
DEFINE_RPR_XML_TAG( unit_temperature );
DEFINE_RPR_XML_TAG( unit_pressure );
DEFINE_RPR_XML_TAG( unit_surface_heat_flux );
DEFINE_RPR_XML_TAG( unit_convec_coef );
DEFINE_RPR_XML_TAG( file_temperature );
DEFINE_RPR_XML_TAG( file_pressure );
DEFINE_RPR_XML_TAG( file );
DEFINE_RPR_XML_TAG( file_pressure_temperature );
DEFINE_RPR_XML_TAG( file_surface_temperature );
DEFINE_RPR_XML_TAG( file_surface_heat_flux );
DEFINE_RPR_XML_TAG( file_surface_heat_flux_surface_temperatue );
DEFINE_RPR_XML_TAG( file_htc );
DEFINE_RPR_XML_TAG( step );
DEFINE_RPR_XML_TAG( start );
DEFINE_RPR_XML_TAG( steps );
DEFINE_RPR_XML_TAG( file_str_num );
DEFINE_RPR_XML_TAG( data );
DEFINE_RPR_XML_TAG( set );
DEFINE_RPR_XML_TAG( surface );
DEFINE_RPR_XML_TAG( part );
DEFINE_RPR_XML_TAG( export_type );
DEFINE_RPR_XML_TAG( path );
DEFINE_RPR_XML_TAG( path_elems );
DEFINE_RPR_XML_TAG( template_id );
DEFINE_RPR_XML_TAG( name_template );
DEFINE_RPR_XML_TAG( text );
DEFINE_RPR_XML_TAG( element ); 

//feature
DEFINE_RPR_XML_TAG( feature );
DEFINE_RPR_XML_TAG( res_file_key );
DEFINE_RPR_XML_TAG( x_form );
DEFINE_RPR_XML_TAG( origin );
DEFINE_RPR_XML_TAG( translate );
DEFINE_RPR_XML_TAG( global );
DEFINE_RPR_XML_TAG( geometry_version );
DEFINE_RPR_XML_TAG( geom_db_uuid );
DEFINE_RPR_XML_TAG( gdb_light_uuid );
DEFINE_RPR_XML_TAG( geom_ref );
DEFINE_RPR_XML_TAG( geom_ref_section );
DEFINE_RPR_XML_TAG( equation );
DEFINE_RPR_XML_TAG( section_plane );
DEFINE_RPR_XML_TAG( section_planes );
DEFINE_RPR_XML_TAG( curve_data );

DEFINE_RPR_XML_TAG( use_cad_geom );
DEFINE_RPR_XML_TAG( moving_reference );
DEFINE_RPR_XML_TAG( moving_display );
DEFINE_RPR_XML_TAG( display_bl );
DEFINE_RPR_XML_TAG( interpolate );

//palette
DEFINE_RPR_XML_TAG( color_palette );
DEFINE_RPR_XML_TAG( outer_minmax_coloring );
DEFINE_RPR_XML_TAG( outer_min_color );
DEFINE_RPR_XML_TAG( outer_max_color );
DEFINE_RPR_XML_TAG( outer_colors_display );
DEFINE_RPR_XML_TAG( display_plot_names );
DEFINE_RPR_XML_TAG( display_plot_minmaxes );
DEFINE_RPR_XML_TAG( display_title );
DEFINE_RPR_XML_TAG( use_default_font );
DEFINE_RPR_XML_TAG( auto_scale_font );
DEFINE_RPR_XML_TAG( bkgd_color );
DEFINE_RPR_XML_TAG( bkgd_bottom_color );
DEFINE_RPR_XML_TAG( bkgd_transp );
DEFINE_RPR_XML_TAG( auto_ticks );
DEFINE_RPR_XML_TAG( logarithmic );
DEFINE_RPR_XML_TAG( ticks_number );
DEFINE_RPR_XML_TAG( param_min );
DEFINE_RPR_XML_TAG( param_max );
DEFINE_RPR_XML_TAG( vertical );
DEFINE_RPR_XML_TAG( rot_text );
DEFINE_RPR_XML_TAG( custom_params_mins );
DEFINE_RPR_XML_TAG( custom_params_maxs );
DEFINE_RPR_XML_TAG( sharable );
DEFINE_RPR_XML_TAG( single_param );
DEFINE_RPR_XML_TAG( enabled_pmm );
DEFINE_RPR_XML_TAG( enabled_outer_colors );
DEFINE_RPR_XML_TAG( is_user_pos );

//geom plot
DEFINE_RPR_XML_TAG( contours );
DEFINE_RPR_XML_TAG( isolines );
DEFINE_RPR_XML_TAG( vectors );
DEFINE_RPR_XML_TAG( build_palette );
DEFINE_RPR_XML_TAG( vectors_projected );
DEFINE_RPR_XML_TAG( minmax_arrowsize_ratio );
DEFINE_RPR_XML_TAG( logarithmic_arrows );
DEFINE_RPR_XML_TAG( essslic );
DEFINE_RPR_XML_TAG( vistype );
DEFINE_RPR_XML_TAG( esss_vistype );
DEFINE_RPR_XML_TAG( esss_linespacing );
DEFINE_RPR_XML_TAG( esss_linewidth );
DEFINE_RPR_XML_TAG( lic_noisegrainsize );
DEFINE_RPR_XML_TAG( lic_noisedensity );
DEFINE_RPR_XML_TAG( lic_contrast );
DEFINE_RPR_XML_TAG( lic_convsize );
DEFINE_RPR_XML_TAG( mesh );
DEFINE_RPR_XML_TAG( background );
DEFINE_RPR_XML_TAG( outlines_display );
DEFINE_RPR_XML_TAG( outlines_color );
DEFINE_RPR_XML_TAG( flip );
DEFINE_RPR_XML_TAG( plane_geom_ref );
DEFINE_RPR_XML_TAG( profile );
DEFINE_RPR_XML_TAG( bumpsize );
DEFINE_RPR_XML_TAG( isolines_displayvalues );
DEFINE_RPR_XML_TAG( vectors_uniformmesh );
DEFINE_RPR_XML_TAG( section_def_mode );
DEFINE_RPR_XML_TAG( xyz_plane );
DEFINE_RPR_XML_TAG( cut_with_section );
DEFINE_RPR_XML_TAG( aspect );
DEFINE_RPR_XML_TAG( percentage );
DEFINE_RPR_XML_TAG( levels );
DEFINE_RPR_XML_TAG( channels );

//goal plot
DEFINE_RPR_XML_TAG( goal_uuid );
DEFINE_RPR_XML_TAG( goal_name );
DEFINE_RPR_XML_TAG( abscissa );
DEFINE_RPR_XML_TAG( chart );
DEFINE_RPR_XML_TAG( minmax );
DEFINE_RPR_XML_TAG( x_min );
DEFINE_RPR_XML_TAG( x_max );
DEFINE_RPR_XML_TAG( y_min );
DEFINE_RPR_XML_TAG( y_max );
DEFINE_RPR_XML_TAG( fit );

//params
DEFINE_RPR_XML_TAG( parameter );
DEFINE_RPR_XML_TAG( parameters );
DEFINE_RPR_XML_TAG( fld );
DEFINE_RPR_XML_TAG( parameters_filter );
DEFINE_RPR_XML_TAG( reset_max );
DEFINE_RPR_XML_TAG( reset_min );
DEFINE_RPR_XML_TAG( cur_parameter );

//sp params
DEFINE_RPR_XML_TAG( full_task );
DEFINE_RPR_XML_TAG( separation );
DEFINE_RPR_XML_TAG( sort_by_parameter );
DEFINE_RPR_XML_TAG( display_parameters );
DEFINE_RPR_XML_TAG( ref_define_mode );

//points
DEFINE_RPR_XML_TAG( point );
DEFINE_RPR_XML_TAG( points );
DEFINE_RPR_XML_TAG( x );
DEFINE_RPR_XML_TAG( y );
DEFINE_RPR_XML_TAG( z );
DEFINE_RPR_XML_TAG( media );
DEFINE_RPR_XML_TAG( vwr_media );
DEFINE_RPR_XML_TAG( medias );
DEFINE_RPR_XML_TAG( group_by_param );
DEFINE_RPR_XML_TAG( points_def_mode );
DEFINE_RPR_XML_TAG( points_count );
DEFINE_RPR_XML_TAG( points_spacing );
DEFINE_RPR_XML_TAG( points_table_def_mode );
DEFINE_RPR_XML_TAG( points_analytic_def_mode );
DEFINE_RPR_XML_TAG( points_analytic_def_line );
DEFINE_RPR_XML_TAG( points_analytic_def_rect );
DEFINE_RPR_XML_TAG( use_mesh_based_point_generation );

//xy plot
DEFINE_RPR_XML_TAG( resolution );
DEFINE_RPR_XML_TAG( fixed_points );
DEFINE_RPR_XML_TAG( fixed_points_count );
DEFINE_RPR_XML_TAG( merge_lines );

//font
DEFINE_RPR_XML_TAG( font );
DEFINE_RPR_XML_TAG( font_isbold );
DEFINE_RPR_XML_TAG( font_isitalic );

//FTPIBase
DEFINE_RPR_XML_TAG( fixed_color );
DEFINE_RPR_XML_TAG( draw_mode );
DEFINE_RPR_XML_TAG( line_width );
DEFINE_RPR_XML_TAG( cross_size );

//Flow trajectory
DEFINE_RPR_XML_TAG( fluid_uuid );
DEFINE_RPR_XML_TAG( particle_uuid );
DEFINE_RPR_XML_TAG( fluid_res_uuid );
DEFINE_RPR_XML_TAG( particle_res_uuid );
DEFINE_RPR_XML_TAG( inj_name );

//surf plot
DEFINE_RPR_XML_TAG( use_all_faces );
DEFINE_RPR_XML_TAG( use_offset );

//particle study
DEFINE_RPR_XML_TAG( bc_type );
DEFINE_RPR_XML_TAG( diameter );
DEFINE_RPR_XML_TAG( abs_temperature );
DEFINE_RPR_XML_TAG( rel_temperature );
DEFINE_RPR_XML_TAG( rel_velocity );
DEFINE_RPR_XML_TAG( abs_velocity );
DEFINE_RPR_XML_TAG( temperature_type );
DEFINE_RPR_XML_TAG( velocity_type );
DEFINE_RPR_XML_TAG( mass_flow_rate );
DEFINE_RPR_XML_TAG( index );
DEFINE_RPR_XML_TAG( max_length );
DEFINE_RPR_XML_TAG( max_time );
DEFINE_RPR_XML_TAG( max_t_step );
DEFINE_RPR_XML_TAG( relative_to_abs_frame);
DEFINE_RPR_XML_TAG( flow_trajectories_type );
DEFINE_RPR_XML_TAG( traj_type );
DEFINE_RPR_XML_TAG( dyn_traj_style );
DEFINE_RPR_XML_TAG( dyn_traj_width );
DEFINE_RPR_XML_TAG( dyn_traj_spacing );
DEFINE_RPR_XML_TAG( dyn_traj_vel_min );
DEFINE_RPR_XML_TAG( dyn_traj_vel_max );
DEFINE_RPR_XML_TAG( direction );
DEFINE_RPR_XML_TAG( source_mode );
DEFINE_RPR_XML_TAG( default_ );
DEFINE_RPR_XML_TAG( dependency_type );
DEFINE_RPR_XML_TAG( formula );
DEFINE_RPR_XML_TAG( formula2 );
DEFINE_RPR_XML_TAG( formula3 );
DEFINE_RPR_XML_TAG( unit );
DEFINE_RPR_XML_TAG( initial_condition );
DEFINE_RPR_XML_TAG( injection );
DEFINE_RPR_XML_TAG( boundary_condition );
DEFINE_RPR_XML_TAG( fast_process );
DEFINE_RPR_XML_TAG( gravity );
DEFINE_RPR_XML_TAG( computational_domain );
DEFINE_RPR_XML_TAG( gravity_vector );
DEFINE_RPR_XML_TAG( accretion_rate );
DEFINE_RPR_XML_TAG( erosion_rate );
DEFINE_RPR_XML_TAG( save_to_ASCII );
DEFINE_RPR_XML_TAG( ASCII_file );
DEFINE_RPR_XML_TAG( massless );
DEFINE_RPR_XML_TAG( project );
DEFINE_RPR_XML_TAG( project_data );
DEFINE_RPR_XML_TAG( configuration_data );
DEFINE_RPR_XML_TAG( material );
DEFINE_RPR_XML_TAG( material_type );
DEFINE_RPR_XML_TAG( ps_uuid );
DEFINE_RPR_XML_TAG( file_path );
DEFINE_RPR_XML_TAG( file_time );

//3dmesh
DEFINE_RPR_XML_TAG( list_uuids );
DEFINE_RPR_XML_TAG( cell_volumes );
DEFINE_RPR_XML_TAG( brick_type_opt_map );
DEFINE_RPR_XML_TAG( directory );
DEFINE_RPR_XML_TAG( cell_mode );
DEFINE_RPR_XML_TAG( mode );
DEFINE_RPR_XML_TAG( section_medias );
DEFINE_RPR_XML_TAG( surface_media );

DEFINE_RPR_XML_TAG( adv_mesh );
DEFINE_RPR_XML_TAG( apply_lighting );
DEFINE_RPR_XML_TAG( HQ_highlight );
DEFINE_RPR_XML_TAG( highlight_color );
DEFINE_RPR_XML_TAG( select_color );
DEFINE_RPR_XML_TAG( task_attributes );

//rays
DEFINE_RPR_XML_TAG( min_wavelength );
DEFINE_RPR_XML_TAG( max_wavelength );
DEFINE_RPR_XML_TAG( min_band_number );
DEFINE_RPR_XML_TAG( max_band_number );
DEFINE_RPR_XML_TAG( start_crop );
DEFINE_RPR_XML_TAG( reach_crop );
DEFINE_RPR_XML_TAG( use_start_crop );
DEFINE_RPR_XML_TAG( use_reach_crop );
DEFINE_RPR_XML_TAG( sources );
DEFINE_RPR_XML_TAG( show_forward );
DEFINE_RPR_XML_TAG( show_backward );

//noise prediction
DEFINE_RPR_XML_TAG( result_type );

//task attributes
DEFINE_RPR_XML_TAG(arrFSubst);
DEFINE_RPR_XML_TAG(SubstUUID);
DEFINE_RPR_XML_TAG(SubstNAME);
DEFINE_RPR_XML_TAG(AllowedMF);
DEFINE_RPR_XML_TAG(AllowedVF);
DEFINE_RPR_XML_TAG(AllowedF);

DEFINE_RPR_XML_TAG(arrCombustionProductsAllowed);
DEFINE_RPR_XML_TAG(arrSubstUUIDs_immi);
DEFINE_RPR_XML_TAG(arrSubstNAMESs_immi);
DEFINE_RPR_XML_TAG(b_SUPPORT);
DEFINE_RPR_XML_TAG(b_CPT_only);
DEFINE_RPR_XML_TAG(b_Use_surface);
DEFINE_RPR_XML_TAG(b_HMN);
DEFINE_RPR_XML_TAG(b_GAS);
DEFINE_RPR_XML_TAG(b_TURBULENT);
DEFINE_RPR_XML_TAG(b_CHT);
DEFINE_RPR_XML_TAG(eRadTtype);
DEFINE_RPR_XML_TAG(RadSpectrumDef);
DEFINE_RPR_XML_TAG(BandsNumber);
DEFINE_RPR_XML_TAG(b_NOX);
DEFINE_RPR_XML_TAG(b_SteamAbsoptionInSolids);
DEFINE_RPR_XML_TAG(NO_FLUID);
DEFINE_RPR_XML_TAG(b_ROTATION);
DEFINE_RPR_XML_TAG(b_STEAM);
DEFINE_RPR_XML_TAG(b_RG_GENERAL);
DEFINE_RPR_XML_TAG(b_CAVITATION);
DEFINE_RPR_XML_TAG(b_FreeSurface);
DEFINE_RPR_XML_TAG(b_RealGas);
DEFINE_RPR_XML_TAG(iPorous);
DEFINE_RPR_XML_TAG(b_JH);
DEFINE_RPR_XML_TAG(b_LMA);
DEFINE_RPR_XML_TAG(b_NON_NEWTON);
DEFINE_RPR_XML_TAG(b_COMBUSTION);
DEFINE_RPR_XML_TAG(HVAC_COMFORT_PARAMETERS);
DEFINE_RPR_XML_TAG(b_LEDAny);
DEFINE_RPR_XML_TAG(b_NetworkAssembly);
DEFINE_RPR_XML_TAG(b_LEDStructFunc);
DEFINE_RPR_XML_TAG(bIsSurfaceCondensation);
DEFINE_RPR_XML_TAG(b_UVGI);
DEFINE_RPR_XML_TAG(Any2RFeatures);

DEFINE_RPR_XML_TAG(support);
